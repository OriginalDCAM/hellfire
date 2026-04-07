//
// Created by denzel on 29/09/2025.
//
#include "hellfire/graphics/lighting/DirectionalLight.h"

#include "hellfire/scene/Scene.h"

namespace hellfire {
    EntityID DirectionalLight::create(Scene *scene, const std::string &name, const glm::vec3 &direction,
        const glm::vec3 &color, float intensity) {
        EntityID id = scene->create_entity(name);
        Entity* entity = scene->get_entity(id);
        
        auto* light = entity->add_component<LightComponent>(LightComponent::LightType::DIRECTIONAL);
        light->set_direction(direction);
        light->set_color(color);
        light->set_intensity(intensity);
        
        return id;
    }
}
