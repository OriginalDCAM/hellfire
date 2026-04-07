//
// Created by denzel on 05/06/2025.
//
#include "Objects/DisplayCase.hpp"

#include "Dcraft/Graphics/Primitives/Cube.h"

void DisplayCase::setup() {
    // Create the material
    auto GlassMaterialComponent = DCraft::MaterialBuilder::create_phong("Glass Material");
    GlassMaterialComponent->set_texture("assets/textures/transparent/glass.jpg", DCraft::TextureType::DIFFUSE);
    GlassMaterialComponent->set_uv_tiling(1.0f, 1.0f);
    GlassMaterialComponent->set_ambient_color(glm::vec3(0.5, 0.5, 0.5));
    GlassMaterialComponent->set_specular_color(glm::vec3(0.5, 0.5, 0.5));
    GlassMaterialComponent->set_shininess(64.0f);
    GlassMaterialComponent->set_transparency(0.6f); 

    uint32_t transparent_shader = DCraft::Application::get_instance().get_shader_manager().get_shader_for_material(*GlassMaterialComponent);
    // uint32_t custom_test_shader = DCraft::Application::get_instance().get_shader_manager().load_shader_from_files("assets/shaders/standard.vert", "assets/shaders/test.frag");
    GlassMaterialComponent->set_compiled_shader_id(transparent_shader);

    // Create the mesh
    DCraft::Cube* MeshComponent = new DCraft::Cube("Glass Cube Mesh");
    MeshComponent->set_rotation(glm::vec3(90.0f, 0.0f, 0.0f));
    MeshComponent->set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
    MeshComponent->set_position(0.0f, 4.0f, 0.0f);
    MeshComponent->set_material(GlassMaterialComponent.release());
    
    // Add it to this object
    add(MeshComponent);
}

void DisplayCase::update(float delta_time) {
    // Rotate the mesh
    auto movement_speed = 5 * delta_time;
    set_rotation(glm::vec3(0,movement_speed,0) + get_rotation());
}
