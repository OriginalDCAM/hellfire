//
// Created by denzel on 18/10/2025.
//

#pragma once
#include <vector>
#include <string>

#include "hellfire/graphics/material/Material.h"
#include "hellfire/graphics/material/MaterialConstants.h"
#include "hellfire/graphics/texture/Texture.h"

namespace hellfire {
    /**
     * Manages all possible texture slots for materials
     * Ensures UI can show all texture options even if not currently assigned
     */
    class TextureSlotManager {
    public:
        struct SlotInfo {
            const char* uniform_name;
            const char* flag_name;
            const char* display_name;
            TextureType type;
            
            SlotInfo(const char* uniform, const char* flag, const char* display, TextureType t)
                : uniform_name(uniform), flag_name(flag), display_name(display), type(t) {}
        };

        // Get all available texture slots
        static const std::vector<SlotInfo>& get_all_slots() {
            static const std::vector<SlotInfo> slots = {
                {
                    MaterialConstants::DIFFUSE_TEXTURE,
                    MaterialConstants::USE_DIFFUSE_TEXTURE,
                    "Diffuse",
                    TextureType::DIFFUSE
                },
                {
                    MaterialConstants::NORMAL_TEXTURE,
                    MaterialConstants::USE_NORMAL_TEXTURE,
                    "Normal Map",
                    TextureType::NORMAL
                },
                {
                    MaterialConstants::SPECULAR_TEXTURE,
                    MaterialConstants::USE_SPECULAR_TEXTURE,
                    "Specular",
                    TextureType::SPECULAR
                },
                {
                    MaterialConstants::METALLIC_TEXTURE,
                    MaterialConstants::USE_METALLIC_TEXTURE,
                    "Metallic",
                    TextureType::METALNESS
                },
                {
                    MaterialConstants::ROUGHNESS_TEXTURE,
                    MaterialConstants::USE_ROUGHNESS_TEXTURE,
                    "Roughness",
                    TextureType::ROUGHNESS
                },
                {
                    MaterialConstants::AO_TEXTURE,
                    MaterialConstants::USE_AO_TEXTURE,
                    "Ambient Occlusion",
                    TextureType::AMBIENT_OCCLUSION
                },
                {
                    MaterialConstants::EMISSIVE_TEXTURE,
                    MaterialConstants::USE_EMISSIVE_TEXTURE,
                    "Emissive",
                    TextureType::EMISSIVE
                }
            };
            return slots;
        }

        // Get PBR-specific slots (subset)
        static std::vector<SlotInfo> get_pbr_slots() {
            auto all = get_all_slots();
            return {
                all[0],  // Diffuse
                all[1],  // Normal
                all[3],  // Metallic
                all[4],  // Roughness
                all[5],  // AO
                all[6]   // Emissive
            };
        }

        // Get basic/legacy slots
        static std::vector<SlotInfo> get_basic_slots() {
            auto all = get_all_slots();
            return {
                all[0],  // Diffuse
                all[1],  // Normal
                all[2]   // Specular
            };
        }

        // Check if a material has a specific texture slot enabled
        static bool is_slot_enabled(const Material& material, const SlotInfo& slot) {
            return material.get_property<bool>(slot.flag_name, false);
        }

        // Get texture from a specific slot (may return nullptr)
        static Texture* get_slot_texture(const Material& material, const SlotInfo& slot) {
            return material.get_property<Texture*>(slot.uniform_name, nullptr);
        }

        // Enable a texture slot with a new texture
        static void enable_slot(Material& material, const SlotInfo& slot, const std::string& texture_path) {
            material.set_texture(texture_path, slot.type, 0);
        }

        // Disable a texture slot
        static void disable_slot(Material& material, const SlotInfo& slot, const bool should_clear_texture = false) {
            material.set_property(slot.flag_name, false);

            if (should_clear_texture) {
                material.set_property(slot.uniform_name, static_cast<Texture*>(nullptr));
            }
        }
    };
}