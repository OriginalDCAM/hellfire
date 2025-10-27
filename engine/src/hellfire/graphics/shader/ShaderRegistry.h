//
// Created by denzel on 14/08/2025.
//

#pragma once
#include "Shader.h"
#include "hellfire/graphics/managers/ShaderManager.h"

namespace hellfire {
    class ShaderRegistry {
    public:
        ShaderRegistry(ShaderManager* manager) : shader_manager_(manager) {}

        // Get a Shader wrapper for convenient uniform setting
        Shader* get_shader(const std::string& name) {
            // Check if we already have a wrapper
            auto it = shader_wrappers_.find(name);
            if (it != shader_wrappers_.end()) {
                return it->second.get();
            }

            // Try to get the compiled program ID from manager
            uint32_t program_id = shader_manager_->get_shader(name);
            if (program_id == 0) {
                return nullptr;
            }

            // Create wrapper and cache it
            auto shader_wrapper = std::make_unique<Shader>(program_id);
            Shader* ptr = shader_wrapper.get();
            shader_wrappers_[name] = std::move(shader_wrapper);
            return ptr;
        }

        Shader* get_shader_from_id(uint32_t program_id) {
            if (program_id == 0) return nullptr;
        
            // Check if we already have a wrapper for this ID
            auto it = id_to_shader_map_.find(program_id);
            if (it != id_to_shader_map_.end()) {
                return it->second;
            }

            // Create a new wrapper for this ID
            auto shader_wrapper = std::make_unique<Shader>(program_id);
            Shader* ptr = shader_wrapper.get();
        
            // Cache it with a generated key (since we don't know the original name)
            std::string generated_key = "id_" + std::to_string(program_id);
            shader_wrappers_[generated_key] = std::move(shader_wrapper);
            id_to_shader_map_[program_id] = ptr;
        
            return ptr;
        }

        // Load and get shader in one call
        Shader* load_and_get_shader(const ShaderManager::ShaderVariant& variant) {
            std::string key = variant.get_key();
            uint32_t program_id = shader_manager_->load_shader(variant);
        
            if (program_id == 0) {
                return nullptr;
            }

            auto shader_wrapper = std::make_unique<Shader>(program_id);
            Shader* ptr = shader_wrapper.get();
            shader_wrappers_[key] = std::move(shader_wrapper);
            return ptr;
        }

        // Convenience method
        Shader* load_and_get_shader(const std::string& vertex_path, const std::string& fragment_path) {
            ShaderManager::ShaderVariant variant;
            variant.vertex_path = vertex_path;
            variant.fragment_path = fragment_path;
            return load_and_get_shader(variant);
        }

        void clear() {
            shader_wrappers_.clear();
        }

        
    private:
        ShaderManager* shader_manager_;
        std::unordered_map<std::string, std::unique_ptr<Shader>> shader_wrappers_;
        std::unordered_map<uint32_t, Shader*> id_to_shader_map_; 
    };
}
