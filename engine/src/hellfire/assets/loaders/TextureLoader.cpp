//
// Created by denzel on 07/04/2026.
//

#include "TextureLoader.h"

#include <fstream>
#include <iostream>

#include "hellfire/assets/AssetManager.h"
#include "stb_image.h"

namespace hellfire {
// Helpers
GLint get_gl_wrap_mode(const TextureWrap wrap) {
  switch (wrap) {
    case TextureWrap::REPEAT: return GL_REPEAT;
    case TextureWrap::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
    case TextureWrap::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
    case TextureWrap::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
    default: return GL_REPEAT;
  }
}

GLint get_gl_filter_mode(const TextureFilter filter) {
  switch (filter) {
    case TextureFilter::NEAREST: return GL_NEAREST;
    case TextureFilter::LINEAR: return GL_LINEAR;
    case TextureFilter::LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
    case TextureFilter::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
    default: return GL_LINEAR;
  }
}



std::optional<ImageData> TextureLoader::load_from_disk(
    const std::filesystem::path &path, int desired_channels) {
  int width = 0;
  int height = 0;
  int nr_channels = 0;

  // Check if file exists
  if (const std::ifstream file(path); !file.good()) {
    std::cerr << "Texture file does not exist at: " << path << std::endl;
    return std::nullopt;
  }

  // Set the stbi settings
  unsigned char *data = stbi_load(path.string().c_str(), &width, &height,
                                  &nr_channels, desired_channels);

  if (!data) {
    const char *error = stbi_failure_reason();
    std::cerr << "stbi failed to load: " << path << " - "
              << (error ? error : "Unknown error") << std::endl;
    return std::nullopt;
  }

  // Validate the loaded image data
  if (width <= 0 || height <= 0 || nr_channels <= 0) {
    std::cerr << "Invalid texture data: " << width << "x" << height
              << " channels=" << nr_channels << std::endl;
    stbi_image_free(data);
    return std::nullopt;
  }

  if (desired_channels > 0) {
    nr_channels = desired_channels;
  }

  return ImageData{data, width, height, nr_channels};
}

std::unique_ptr<Texture> TextureLoader::upload_to_gpu(
    const ImageData &image, const TextureType type, const TextureSettings &settings) {
  TextureHandle handle;

  // Generate OpenGL tex
  glGenTextures(1, &handle);
  if (handle == 0) {  // Handle invalid handle case
    std::cerr << "Failed to generate texture handle " << std::endl;
    return nullptr;
  }

  glBindTexture(GL_TEXTURE_2D, handle);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Determine formats
  GLenum format, internal_format;
  switch (image.channels) {
    case 1:
      format = GL_RED;
      internal_format = GL_R8;
      break;
    case 3:
      format = GL_RGB;
      internal_format = GL_RGB8;
      break;
    case 4:
      format = GL_RGBA;
      internal_format = GL_RGBA8;
      break;
    default:
      std::cerr << "Unsupported channel count: " << image.channels << std::endl;
      glDeleteTextures(1, &handle);
      return nullptr;
  }

  // Handle special texture types
  if (type == TextureType::ROUGHNESS || type == TextureType::METALNESS ||
      type == TextureType::AMBIENT_OCCLUSION) {
    if (image.channels >= 3) {
      format = GL_RED;          // Read only red channel from source
      internal_format = GL_R8;  // Store as single channel
    }
  }
  
  // Upload to GPU
  glTexImage2D(GL_TEXTURE_2D, 0, internal_format, image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.pixels.get());
  
  GLenum gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    glDeleteTextures(1, &handle);
    handle = 0;
    return nullptr;
  }
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, get_gl_wrap_mode(settings.wrap_s));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, get_gl_wrap_mode(settings.wrap_t));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, get_gl_filter_mode(settings.min_filter));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, get_gl_filter_mode(settings.mag_filter));

  auto tex = std::make_unique<Texture>(handle, type, image.width, image.height, image.channels);
  return std::move(tex);
}

std::unique_ptr<Texture> TextureLoader::load(const std::filesystem::path &path,
                                             TextureType type,
                                             const TextureSettings &settings) {
  const auto image_data = load_from_disk(path, settings.desired_channels);
  if (!image_data) return nullptr;
  return upload_to_gpu(image_data.value(), type, settings);
}



}  // namespace hellfire