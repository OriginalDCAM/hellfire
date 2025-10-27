//
// Created by denzel on 17/10/2025.
//

#pragma once
#include "hellfire/graphics/texture/Texture.h"

namespace hellfire {
    /**
     * Centralized material property and uniform names
     * This eliminates magic strings throughout the codebase
     */
    struct MaterialConstants {
        // === Color Properties ===
        static constexpr const char* DIFFUSE_COLOR = "uDiffuseColor";
        static constexpr const char* AMBIENT_COLOR = "uAmbientColor";
        static constexpr const char* SPECULAR_COLOR = "uSpecularColor";
        static constexpr const char* EMISSIVE_COLOR = "uEmissiveColor";
        
        // === Texture Properties ===
        static constexpr const char* DIFFUSE_TEXTURE = "uDiffuseTexture";
        static constexpr const char* NORMAL_TEXTURE = "uNormalTexture";
        static constexpr const char* SPECULAR_TEXTURE = "uSpecularTexture";
        static constexpr const char* METALLIC_TEXTURE = "uMetallicTexture";
        static constexpr const char* ROUGHNESS_TEXTURE = "uRoughnessTexture";
        static constexpr const char* AO_TEXTURE = "uAoTexture";
        static constexpr const char* EMISSIVE_TEXTURE = "uEmissiveTexture";
        
        // === Texture Usage Flags ===
        static constexpr const char* USE_DIFFUSE_TEXTURE = "useUDiffuseTexture";
        static constexpr const char* USE_NORMAL_TEXTURE = "useUNormalTexture";
        static constexpr const char* USE_SPECULAR_TEXTURE = "useUSpecularTexture";
        static constexpr const char* USE_METALLIC_TEXTURE = "useUMetallicTexture";
        static constexpr const char* USE_ROUGHNESS_TEXTURE = "useURoughnessTexture";
        static constexpr const char* USE_AO_TEXTURE = "useUAoTexture";
        static constexpr const char* USE_EMISSIVE_TEXTURE = "useUEmissiveTexture";
        
        // === Material Properties ===
        static constexpr const char* SHININESS = "uShininess";
        static constexpr const char* METALLIC = "uMetallic";
        static constexpr const char* ROUGHNESS = "uRoughness";
        static constexpr const char* OPACITY = "uOpacity";
        
        // === UV Transform Properties ===
        static constexpr const char* UV_TILING = "uvTiling";
        static constexpr const char* UV_OFFSET = "uvOffset";
        static constexpr const char* UV_ROTATION = "uvRotation";
        
        // === Texture Type to Uniform Name Mapping ===
        static const char* get_texture_uniform_name(TextureType type) {
            switch (type) {
                case TextureType::DIFFUSE: return DIFFUSE_TEXTURE;
                case TextureType::NORMAL: return NORMAL_TEXTURE;
                case TextureType::SPECULAR: return SPECULAR_TEXTURE;
                case TextureType::METALNESS: return METALLIC_TEXTURE;
                case TextureType::ROUGHNESS: return ROUGHNESS_TEXTURE;
                case TextureType::AMBIENT_OCCLUSION: return AO_TEXTURE;
                case TextureType::EMISSIVE: return EMISSIVE_TEXTURE;
                default: return nullptr;
            }
        }
        
        static const char* get_texture_flag_name(TextureType type) {
            switch (type) {
                case TextureType::DIFFUSE: return USE_DIFFUSE_TEXTURE;
                case TextureType::NORMAL: return USE_NORMAL_TEXTURE;
                case TextureType::SPECULAR: return USE_SPECULAR_TEXTURE;
                case TextureType::METALNESS: return USE_METALLIC_TEXTURE;
                case TextureType::ROUGHNESS: return USE_ROUGHNESS_TEXTURE;
                case TextureType::AMBIENT_OCCLUSION: return USE_AO_TEXTURE;
                case TextureType::EMISSIVE: return USE_EMISSIVE_TEXTURE;
                default: return nullptr;
            }
        }
    };
}
