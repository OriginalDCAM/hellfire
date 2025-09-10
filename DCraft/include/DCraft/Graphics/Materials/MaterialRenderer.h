//
// MaterialRenderer.h - Material binding for your Material class
//
#pragma once
#include <string>

#include "Material.h"

namespace DCraft {
    class Material;

    class MaterialRenderer {
    public:
        static void bind_material(const Material& material);

        static void bind_property_to_shader(const Material::Property &property, uint32_t shader_program, int &texture_unit);

    private:
        static void bind_property(const Material::Property& property, uint32_t shader_program, int& texture_unit);

        static std::string create_use_flag(const std::string &uniform_name);
    };
}
