//
// Created by denzel on 11/04/2025.
//

#ifndef MATERIALSERIALIZER_H
#define MATERIALSERIALIZER_H

#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace DCraft {
    class Material;

    class MaterialSerializer {
    public:
        static json serialize_material(Material* material);
    
        static Material* deserialize_material(const json& data);
    };
}
#endif //MATERIALSERIALIZER_H
