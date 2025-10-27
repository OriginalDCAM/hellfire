//
// Created by denzel on 29/09/2025.
//
#include "hellfire/graphics/lighting/PointLight.h"

#include "hellfire/scene/Scene.h"

namespace hellfire {
    EntityID PointLight::create(Scene *scene, const std::string &name, const glm::vec3 &position,
                                const glm::vec3 &color, float intensity, float range) {
        EntityID id = scene->create_entity(name);
        Entity *entity = scene->get_entity(id);

        auto *light = entity->add_component<LightComponent>(LightComponent::LightType::POINT);
        light->set_color(color);
        light->set_intensity(intensity);
        light->set_range(range);

        entity->transform()->set_position(position);

        return id;
    }
}
