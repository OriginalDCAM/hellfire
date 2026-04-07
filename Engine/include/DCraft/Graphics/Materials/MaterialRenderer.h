//
// MaterialRenderer.h - Material binding for your Material class
//
#pragma once
#include <string>
#include <algorithm>

#include "Material.h"

namespace DCraft {
    class Material;

    class MaterialRenderer {
    public:
        static void bind_material(const Material& material);

    private:
        static void bind_property(const Material::Property& property, uint32_t shader_program, int& texture_unit);

        static std::string capitalize_first(const std::string& str);
    };
}
