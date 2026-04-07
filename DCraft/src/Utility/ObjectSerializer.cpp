//
// Created by denzel on 11/04/2025.
//
#include "DCraft/Utility/ObjectSerializer.h"

#include "DCraft/Graphics/Primitives/Shape3D.h"
#include "DCraft/Utility/MaterialSerializer.h"

namespace DCraft {
    ObjectSerializer::ObjectSerializer(SceneManager *scene_manager)
        : scene_manager_(scene_manager) {
    }

    json ObjectSerializer::serialize_object(Object3D *obj) {
        if (!obj) return json();

        json node_data = obj->to_json();

        if (auto *shape = dynamic_cast<Shape3D *>(obj)) {
            if (Material *material = shape->get_material()) {
                node_data["material"] = MaterialSerializer::serialize_material(material);
            }
        }

        serialize_children(obj, node_data);

        return node_data;
    }

    void ObjectSerializer::serialize_children(Object3D *parent, json &node_data) {
        auto &children = parent->get_children();
        if (!children.empty()) {
            json children_array = json::array();

            for (auto *child: children) {
                if (child) {
                    children_array.push_back(serialize_object(child));
                }
            }

            if (!children_array.empty()) {
                node_data["children"] = children_array;
            }
        }
    }
}
