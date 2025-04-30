//
// Created by denzel on 11/04/2025.
//

#pragma once
#include <nlohmann/json.hpp>
#include "DCraft/Structs/Object3D.h"

namespace DCraft {
    class SceneManager;


    using json = nlohmann::json;
}

namespace DCraft {
    class ObjectSerializer {
    public:
        ObjectSerializer(SceneManager *scene_manager);

        // Serialize an object into JSON data
        json serialize_object(Object3D *obj);

        // Process children of an object
        void serialize_children(Object3D *parent, json &node_data);

    private:
        SceneManager *scene_manager_;

        // Helper to determine if an object has to_json method (can be determined at runtime)
        bool has_to_json_method(Object3D *obj);

        // Helper for objects that don't have to_json
        json fallback_serialize(Object3D *obj);
    };
}
