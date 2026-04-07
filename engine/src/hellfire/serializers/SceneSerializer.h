//
// Created by denzel on 03/12/2025.
//

#pragma once
#include "hellfire/ecs/ComponentRegistry.h"
#include "hellfire/scene/Scene.h"
#include "hellfire/serializers/Serializer.h"

namespace hellfire {
    template<>
    struct Serializer<Scene> {
        using Remap = std::unordered_map<EntityID, EntityID>;
        
        static bool serialize(std::ostream& output, const Scene* scene) {
            if (scene == nullptr) return false;

            nlohmann::ordered_json j;
            j["name"] = scene->get_name();
            j["default_camera"] = scene->get_default_camera_entity_id();
            j["entities"] = nlohmann::ordered_json::array();

            for (const EntityID root_id : scene->get_root_entities()) {
                serialize_entity_recursive(*scene, root_id, j["entities"]);
            }

            output << j.dump(2);
            return output.good();
        }

        static bool deserialize(std::istream& input, Scene* scene) {
            try {
                nlohmann::json j;
                input >> j;

                scene->set_name(j.at("name"));

                Remap id_remap;
                for (const auto& entity_json : j.at("entities")) {
                    create_entity_recursive(*scene, entity_json, INVALID_ENTITY, id_remap);
                }

                if (j.contains("default_camera")) {
                    EntityID old_id = j.at("default_camera");
                    if (id_remap.contains(old_id)) {
                        scene->set_default_camera(id_remap.at(old_id));
                    }
                }

                return true;
            } catch (...) {
                return false;
            }
        }

    private:
        static void serialize_entity_recursive(const Scene& scene, EntityID id, nlohmann::ordered_json& out) {
            const Entity* entity = scene.get_entity(id);
            if (!entity) return;

            nlohmann::ordered_json entity_json;
            entity_json["id"] = id;
            entity_json["name"] = entity->get_name();
            entity_json["components"] = ComponentRegistry::instance().serialize_all_components(*entity);
            entity_json["children"] = nlohmann::json::array();

            for (EntityID child_id : scene.get_children(id)) {
                serialize_entity_recursive(scene, child_id, entity_json["children"]);
            }

            out.push_back(entity_json);
        }

        static void create_entity_recursive(Scene& scene, const nlohmann::json& entity_json, EntityID parent_id, Remap& id_remap) {
            EntityID old_id = entity_json.at("id");
            EntityID new_id = scene.create_entity(entity_json.at("name"));
            id_remap[old_id] = new_id;

            if (parent_id != INVALID_ENTITY) {
                scene.set_parent(new_id, parent_id);
            }

            Entity* entity = scene.get_entity(new_id);
            if (entity_json.contains("components")) {
            ComponentRegistry::instance().deserialize_all_components(*entity, entity_json.at("components"));
            }

            if (entity_json.contains("children")) {
                for (const auto& child_json : entity_json.at("children")) {
                    create_entity_recursive(scene, child_json, new_id, id_remap);
                }
            }
            
            
        }
    };
}
