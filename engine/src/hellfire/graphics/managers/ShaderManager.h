//
// Created by denzel on 27/05/2025.
//
#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <regex>

namespace hellfire {
    class Application;
}

namespace hellfire {
    class Material;

    class ShaderManager {
    private:
        std::unordered_map<std::string, std::string> include_cache_;
        std::unordered_map<std::string, uint32_t> compiled_shaders_;
        
        // Recursively process #include directives
        std::string process_includes(const std::string& source, const std::string& base_path = "shaders/");
        
        std::string load_include_file(const std::string &path, const std::string &base_path);

        std::string get_directory_from_path(const std::string &file_path);

        // Process #ifdef, #ifndef, #define directives
        std::string process_defines(const std::string& source, 
                                   const std::unordered_set<std::string>& defines);
        
        std::string trim(const std::string& str);

        std::string load_shader_file(const std::string& path);

    public:
        struct ShaderVariant {
            std::string vertex_path;
            std::string fragment_path;
            std::unordered_set<std::string> defines;
            
            std::string get_key() const;
        };

        std::string clean_shader_content(const std::string &content, const std::string &filepath);

        uint32_t load_shader(const ShaderVariant& variant);

        // Method for material-based shader loading
        uint32_t get_shader_for_material(Material& material);

        [[nodiscard]] uint32_t get_shader(const std::string& key) const;

        bool has_shader(const std::string& key) const {
            return compiled_shaders_.find(key) != compiled_shaders_.end();
        }

        // Helper method to add automatic defines based on material properties
        void add_automatic_defines(const Material& material, std::unordered_set<std::string>& defines);

        uint32_t load_shader_from_files(const std::string& vertex_path, const std::string& fragment_path);

        void clear_cache();

        ~ShaderManager() {
            // clear_cache();
        }
        
        // Get all compiled shader IDs (for cleanup in Application)
        std::vector<uint32_t> get_all_shader_ids() const;

        friend class Application;

    private:
        uint32_t compile_shader_program(const std::string& vertex_source, const std::string& fragment_source);
    };
}
