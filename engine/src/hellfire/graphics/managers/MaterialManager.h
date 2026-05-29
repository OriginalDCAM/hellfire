//
// MaterialRenderer.h - Material binding for your Material class
//
#pragma once
#include <string>
#include <algorithm>

#include "../material/Material.h"

namespace hellfire {
    class Material;

    class MaterialManager {
    public:
        static void bind_material(const Material& material);
        static void bind_property(const Material::Property& property, uint32_t shader_program, int& texture_unit);
    private:
     static const char* get_texture_flag_for_uniform(const std::string& uniform_name);

     static std::string capitalize_first(const std::string& str);

    };
}
