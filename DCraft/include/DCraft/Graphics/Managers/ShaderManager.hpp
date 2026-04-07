//
// Created by denzel on 27/05/2025.
//
#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <fstream>
#include <sstream>
#include <stack>
#include <iostream>

#include "DCraft/Graphics/Materials/Material.h"
#include "DCraft/Graphics/OGL/glsl.h"
#include <GL/glew.h>

#ifdef _WIN32
#include <windows.h> // For GetModuleFileNameA and MAX_PATH
#endif

namespace DCraft {
    class ShaderManager {
    private:
        std::unordered_map<std::string, std::string> include_cache_;
        std::unordered_map<std::string, uint32_t> compiled_shaders_;
        
        // Recursively process #include directives
        std::string process_includes(const std::string& source, const std::string& base_path = "shaders/") {
            std::string processed = source;
            std::regex include_regex("#include\\s+\"([^\"]+)\"");
            std::smatch match;
            
            while (std::regex_search(processed, match, include_regex)) {
                std::string include_path = base_path + match[1].str();
                std::string include_content = load_include_file(include_path);
                
                // Replace the #include line with the file content
                processed = processed.replace(match.position(), match.length(), include_content);
            }
            
            return processed;
        }
        
        std::string load_include_file(const std::string& path) {
            // Check cache first
            if (include_cache_.find(path) != include_cache_.end()) {
                return include_cache_[path];
            }
            
            std::ifstream file(path);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open include file: " + path);
            }
            
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            
            // Process nested includes
            content = process_includes(content);
            
            // Cache the processed content
            include_cache_[path] = content;
            return content;
        }
        
        // Process #ifdef, #ifndef, #define directives
        std::string process_defines(const std::string& source, 
                                   const std::unordered_set<std::string>& defines) {
            std::istringstream stream(source);
            std::ostringstream result;
            std::string line;
            
            std::stack<bool> condition_stack;
            condition_stack.push(true); // Base condition is always true
            
            while (std::getline(stream, line)) {
                std::string trimmed = trim(line);
                
                if (trimmed.substr(0, 7) == "#ifdef ") {
                    std::string define_name = trimmed.substr(7);
                    bool condition = defines.count(trim(define_name)) > 0;
                    condition_stack.push(condition_stack.top() && condition);
                    continue;
                }
                
                if (trimmed.substr(0, 8) == "#ifndef ") {
                    std::string define_name = trimmed.substr(8);
                    bool condition = defines.count(trim(define_name)) == 0;
                    condition_stack.push(condition_stack.top() && condition);
                    continue;
                }
                
                if (trimmed == "#endif") {
                    if (condition_stack.size() > 1) {
                        condition_stack.pop();
                    }
                    continue;
                }
                
                // Only include line if current condition is true
                if (condition_stack.top()) {
                    result << line << "\n";
                }
            }
            
            return result.str();
        }
        
        std::string trim(const std::string& str) {
            size_t start = str.find_first_not_of(" \t");
            if (start == std::string::npos) return "";
            size_t end = str.find_last_not_of(" \t");
            return str.substr(start, end - start + 1);
        }
        
        std::string load_shader_file(const std::string& path) {
            // Handle path resolution (Windows executable path support)
            std::string base_path;
#ifdef _WIN32
            char exePath[MAX_PATH];
            GetModuleFileNameA(nullptr, exePath, MAX_PATH);
            base_path = std::string(exePath);
            base_path = base_path.substr(0, base_path.find_last_of("\\/")) + "/";
#endif
            
            std::string abs_path = path; //  base_path + path 
            std::cout << "Loading shader from: " << abs_path << std::endl;
            
            // Use your existing glsl::readFile function
            char* file_content = glsl::readFile(abs_path.c_str());
            if (!file_content) {
                throw std::runtime_error("Failed to read shader file: " + abs_path);
            }
            
            std::string content(file_content);
            delete[] file_content; // Important: free the memory
            
            return content;
        }
        
    public:
        struct ShaderVariant {
            std::string vertex_path;
            std::string fragment_path;
            std::unordered_set<std::string> defines;
            
            std::string get_key() const {
                std::string key = vertex_path + "|" + fragment_path + "|";
                for (const auto& define : defines) {
                    key += define + ",";
                }
                return key;
            }
        };
        
        uint32_t load_shader(const ShaderVariant& variant) {
            std::string cache_key = variant.get_key();
            
            // Check if already compiled
            if (compiled_shaders_.find(cache_key) != compiled_shaders_.end()) {
                return compiled_shaders_[cache_key];
            }
            
            try {
                // Load and process vertex shader
                std::string vertex_source = load_shader_file(variant.vertex_path);
                vertex_source = process_includes(vertex_source);
                vertex_source = process_defines(vertex_source, variant.defines);
                
                // Load and process fragment shader
                std::string fragment_source = load_shader_file(variant.fragment_path);
                fragment_source = process_includes(fragment_source);
                fragment_source = process_defines(fragment_source, variant.defines);
                
                // Compile shader (you'll need to implement this using your existing shader compilation)
                uint32_t program = compile_shader_program(vertex_source, fragment_source);
                
                // Cache compiled shader
                compiled_shaders_[cache_key] = program;
                
                return program;
            } catch (const std::exception& e) {
                std::cerr << "Error loading shader: " << e.what() << std::endl;
                return 0;
            }
        }
        
        // Enhanced method for material-based shader loading
        uint32_t get_shader_for_material(Material& material) {
            ShaderVariant variant;
            
            // Check if material has custom shader
            if (material.has_custom_shader()) {
                const auto* shader_info = material.get_shader_info();
                variant.vertex_path = shader_info->vertex_path;
                variant.fragment_path = shader_info->fragment_path;
                variant.defines = shader_info->defines;
                
                // Add automatic defines based on properties
                add_automatic_defines(material, variant.defines);
            } else {
                // Use built-in shader based on material type
                int material_type = material.get_builtin_material_type();
                
                switch (material_type) {
                    case 0: // Lambert
                        variant.vertex_path = "./assets/shaders/standard.vert";
                        variant.fragment_path = "./assets/shaders/lambert.frag";
                        break;
                    case 1: // Phong
                        variant.vertex_path = "./assets/shaders/standard.vert";
                        variant.fragment_path = "./assets/shaders/phong.frag";
                        break;
                    case 2: // PBR
                        variant.vertex_path = "./assets/shaders/standard.vert";
                        variant.fragment_path = "./assets/shaders/pbr.frag";
                        break;
                    default:
                        variant.vertex_path = "./assets/shaders/standard.vert";
                        variant.fragment_path = "./assets/shaders/lambert.frag";
                        break;
                }
                
                // Add automatic defines for built-in shaders
                add_automatic_defines(material, variant.defines);
            }
            
            uint32_t shader_id = load_shader(variant);
            material.set_compiled_shader_id(shader_id);
            return shader_id;
        }
        
        // Helper method to add automatic defines based on material properties
        void add_automatic_defines(const Material& material, std::unordered_set<std::string>& defines) {
            if (material.get_property<Texture*>("diffuseTexture", nullptr)) {
                defines.insert("HAS_DIFFUSE_TEXTURE");
            }
            if (material.get_property<Texture*>("normalTexture", nullptr)) {
                defines.insert("HAS_NORMAL_TEXTURE");
            }
            if (material.get_property<Texture*>("specularTexture", nullptr)) {
                defines.insert("HAS_SPECULAR_TEXTURE");
            }
            if (material.get_property<Texture*>("emissionTexture", nullptr)) {
                defines.insert("HAS_EMISSION_TEXTURE");
            }
            if (material.get_property<Texture*>("roughnessTexture", nullptr)) {
                defines.insert("HAS_ROUGHNESS_TEXTURE");
            }
            if (material.get_property<Texture*>("metallicTexture", nullptr)) {
                defines.insert("HAS_METALLIC_TEXTURE");
            }
        }
        
        // Backwards compatibility method - loads shaders from file paths directly
        uint32_t load_shader_from_files(const std::string& vertex_path, const std::string& fragment_path) {
            try {
                std::string vertex_source = load_shader_file(vertex_path);
                std::string fragment_source = load_shader_file(fragment_path);
                
                uint32_t program_id = compile_shader_program(vertex_source, fragment_source);
                
                if (program_id != 0) {
                    // Track shader for cleanup (using file paths as key)
                    std::string cache_key = vertex_path + "|" + fragment_path;
                    compiled_shaders_[cache_key] = program_id;
                }
                
                return program_id;
            } catch (const std::exception& e) {
                std::cerr << "Error loading shaders: " << e.what() << std::endl;
                return 0;
            }
        }
        
        void clear_cache() {
            include_cache_.clear();
            
            // Clean up compiled shaders
            for (const auto& [key, shader_id] : compiled_shaders_) {
                glDeleteProgram(shader_id);
            }
            compiled_shaders_.clear();
        }
        
        ~ShaderManager() {
            clear_cache();
        }
        
        // Get all compiled shader IDs (for cleanup in Application)
        std::vector<uint32_t> get_all_shader_ids() const {
            std::vector<uint32_t> shader_ids;
            shader_ids.reserve(compiled_shaders_.size());
            for (const auto& [key, id] : compiled_shaders_) {
                shader_ids.push_back(id);
            }
            return shader_ids;
        }
        
    private:
        uint32_t compile_shader_program(const std::string& vertex_source, const std::string& fragment_source) {
            const char* vertex_src = vertex_source.c_str();
            const char* fragment_src = fragment_source.c_str();

            GLuint vertex_shader = glsl::makeVertexShader(vertex_src);
            if (vertex_shader == 0) {
                std::cerr << "Failed to compile vertex shader" << std::endl;
                return 0;
            }

            GLuint fragment_shader = glsl::makeFragmentShader(fragment_src);
            if (fragment_shader == 0) {
                std::cerr << "Failed to compile fragment shader" << std::endl;
                glDeleteShader(vertex_shader);
                return 0;
            }

            uint32_t program_id = glsl::makeShaderProgram(vertex_shader, fragment_shader);

            // Clean up individual shaders after linking
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);

            if (program_id == 0) {
                std::cerr << "Failed to link shader program" << std::endl;
            }

            return program_id;
        }
    };
}
