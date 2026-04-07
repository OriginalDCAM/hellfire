//
// Created by denzel on 03/12/2025.
//
#pragma once

#include <iosfwd>

#include "hellfire/ecs/LightComponent.h"
#include "hellfire/ecs/RenderableComponent.h"
#include "hellfire/ecs/TransformComponent.h"
#include "hellfire/ecs/components/MeshComponent.h"
#include "hellfire/scene/Scene.h"
#include "hellfire/utilities/SerializerUtils.h"

namespace hellfire {
    template<typename T>
    struct Serializer {
        static bool serialize(std::ostream &output, const T *obj);

        static bool deserialize(std::istream &input, T *obj);
    };

    template<>
    struct Serializer<TransformComponent> {
        static bool serialize(std::ostream &output, const TransformComponent *obj) {
            if (obj == nullptr) return false;

            const nlohmann::json j = {
                {"position", obj->get_position()},
                {"rotation", obj->get_rotation()},
                {"scale", obj->get_scale()}
            };

            output << j.dump(4);
            return output.good();
        }

        static bool deserialize(std::istream &input, TransformComponent *obj) {
            try {
                if (obj == nullptr) return false;

                nlohmann::json j;
                input >> j;

                const auto position = json_get_vec3(j, "position");
                const auto rotation = json_get_vec3(j, "rotation");
                const auto scale = json_get_vec3(j, "scale");

                if (!position || !rotation || !scale) return false;

                obj->set_position(*position);
                obj->set_rotation(*rotation);
                obj->set_scale(*scale);

                return true;
            } catch (...) {
                return false;
            }
        }
    };

    template<>
    struct Serializer<MeshComponent> {
        static bool serialize(std::ostream &output, const MeshComponent *obj) {
            if (!obj) return false;

            nlohmann::json j = {
                {"mesh_asset", obj->get_mesh_asset()},
                {"is_wireframe", obj->is_wireframe}

            };

            output << j.dump(4);
            return output.good();
        }

        static bool deserialize(std::istream &input, MeshComponent *obj) {
            if (!obj) return false;

            try {
                nlohmann::json j;
                input >> j;

                obj->set_mesh_asset(j.value("mesh_asset", INVALID_ASSET_ID));
                obj->is_wireframe = j.value("is_wireframe", false);

                return true;
            } catch (...) {
                return false;
            }
        }
    };

    template<>
    struct Serializer<RenderableComponent> {
        static bool serialize(std::ostream &output, const RenderableComponent *obj) {
            if (!obj) return false;

            nlohmann::json j = {
                {"material_asset", obj->get_material_asset()},
                {"cast_shadows", obj->cast_shadows},
                {"receive_shadows", obj->receive_shadows},
                {"visible", obj->visible},
                {"render_layer", obj->render_layer}
            };

            output << j.dump(4);
            return output.good();
        }

        static bool deserialize(std::istream &input, RenderableComponent *obj) {
            if (!obj) return false;

            try {
                nlohmann::json j;
                input >> j;

                obj->set_material_asset(j.value("material_asset", INVALID_ASSET_ID));
                obj->cast_shadows = j.value("cast_shadows", true);
                obj->receive_shadows = j.value("receive_shadows", true);
                obj->visible = j.value("visible", true);
                obj->render_layer = j.value("render_layer", 0u);

                return true;
            } catch (...) {
                return false;
            }
        }
    };

    template<>
    struct Serializer<LightComponent> {
        static bool serialize(std::ostream &output, const LightComponent *obj) {
            if (!obj) return false;

            nlohmann::json j = {
                {"light_type", obj->get_light_type()},
                {"color", obj->get_color()},
                {"intensity", obj->get_intensity()},
                {"should_cast_shadows", obj->should_cast_shadows()},
            };

            if (obj->get_light_type() == LightComponent::POINT) {
                j["range"] = obj->get_range();
                j["attenuation"] = obj->get_attenuation();
            }

            output << j.dump(2);

            return output.good();
        }

        static bool deserialize(std::istream &input, LightComponent *obj) {
            if (!obj) return false;

            try {
                nlohmann::json j;
                input >> j;

                obj->set_light_type(j.value("light_type", LightComponent::POINT));
                obj->set_color(j.value("color", glm::vec3(0)));
                obj->set_intensity(j.value("intensity", 1.0f));
                obj->set_cast_shadows(j.value("should_cast_shadows", false));

                if (obj->get_light_type() == LightComponent::POINT) {
                    obj->set_range(j.value("range", 0.0f));
                    obj->set_attenuation(j.value("attenuation", 0.0f));
                }

                return true;
            } catch (...) {
                return false;
            }
        }
    };
}
