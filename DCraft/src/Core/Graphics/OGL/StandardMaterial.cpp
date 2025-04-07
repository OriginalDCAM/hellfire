//
// Created by denzel on 07/04/2025.
//
#include "DCraft/Graphics/OGL/StandardMaterial.h"

#include <iostream>
#include <GL/glew.h>

#include "DCraft/Graphics/Renderer.h"

namespace DCraft::OGL {
    StandardMaterial::StandardMaterial(const std::string &name) : Material(name) {
    }

    void StandardMaterial::bind(void *renderer_context) {
        uint32_t shader_program;
        if (has_custom_shader()) {
            shader_program = get_shader();
        } else {
            auto ogl_context = static_cast<OGLRendererContext *>(renderer_context);
            shader_program = ogl_context->default_shader_program;
        }

        shader_handle = shader_program;
        
        glUseProgram(shader_program);

        int tex_unit = 0;
        std::unordered_map<TextureType, int> counters;
        for (auto &[type, tex_list]: textures_map_) {
            for (Texture *tex: tex_list) {
                glActiveTexture(GL_TEXTURE0 + tex_unit);

                std::string uniform_name;
                switch (type) {
                    case TextureType::DIFFUSE:
                        uniform_name = "diffuse" + std::to_string(++counters[type]);
                        break;
                    case TextureType::SPECULAR:
                        uniform_name = "specular" + std::to_string(++counters[type]);
                        break;
                    case TextureType::NORMAL:
                        uniform_name = "normal" + std::to_string(++counters[type]);
                        break;
                    case TextureType::ROUGHNESS:
                        uniform_name = "roughness" + std::to_string(++counters[type]);
                        break;
                    case TextureType::METALLNESS:
                        uniform_name = "metalness" + std::to_string(++counters[type]);
                        break;
                    case TextureType::AMBIENT_OCCLUSION:
                        uniform_name = "ao" + std::to_string(++counters[type]);
                        break;
                }

                if (!textures_map_[TextureType::DIFFUSE].empty()) {
                    glUniform1i(glGetUniformLocation(shader_program, "useDiffuse1"), GL_TRUE);
                } else {
                    glUniform1i(glGetUniformLocation(shader_program, "useDiffuse1"), GL_FALSE);
                }

                if (!textures_map_[TextureType::SPECULAR].empty()) {
                    glUniform1i(glGetUniformLocation(shader_program, "useSpecular1"), GL_TRUE);
                }

                if (!textures_map_[TextureType::NORMAL].empty()) {
                    glUniform1i(glGetUniformLocation(shader_program, "useSpecular1"), GL_TRUE);
                }

                // Set sampler to correct texture unit
                glUniform1i(glGetUniformLocation(shader_program, uniform_name.c_str()), tex_unit);
                
                tex->bind();
                tex_unit++;
            }
        }
    }
}

