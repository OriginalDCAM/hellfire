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

        // Much larger coverage to include your plane
        float ortho_size = 50.0f; 
        float near_plane = 0.1f;
        float far_plane = 100.0f; 
    
        glm::mat4 light_projection = glm::ortho(
            -ortho_size, ortho_size,
            -ortho_size, ortho_size,
            near_plane, far_plane
        );
    
        glm::vec3 light_pos = glm::vec3(0.0f, 20.0f, 10.0f);
        glm::vec3 look_at = glm::vec3(0.0f, 0.0f, 0.0f);
    
        glm::mat4 light_view = glm::lookAt(
            light_pos,
            look_at,
            glm::vec3(0.0f, 0.0f, -1.0f)  // Up vector
        );
    
        light->set_light_view_proj_matrix(light_projection * light_view);
        
        return id;
    }
}
