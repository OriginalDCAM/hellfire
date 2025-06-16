//
// Corrected SandboxScene.cpp with new material system
//
#include "DCraft/Addons/ImportedModel3D.h"
#include "DCraft/Addons/ModelLoader.h"
#include "DCraft/Graphics/Model.h"
#include "DCraft/Graphics/Lights/DirectionalLight.h"
#include "DCraft/Graphics/Lights/PointLight.h"
#include "DCraft/Graphics/Primitives/Cube.h"
#include "DCraft/Graphics/Primitives/Quad.h"
#include "Objects/DisplayCase.hpp"
#include "Scenes/SandboxScene.h"

MaterialMap load_material_map(DCraft::ShaderManager& shader_manager) {
    MaterialMap materials;
    
    // Create Lambert materials using MaterialBuilder
    auto mossy_material = DCraft::MaterialBuilder::create_lambert("Mossy");
    mossy_material->set_texture("assets/textures/transparent/glass.jpg", DCraft::TextureType::DIFFUSE);
    mossy_material->set_uv_tiling(1.0f, 1.0f);
    mossy_material->set_ambient_color(glm::vec3(0.5, 0.5, 0.5));
    mossy_material->set_transparency(0.50f);
    
    uint32_t mossy_shader = shader_manager.get_shader_for_material(*mossy_material);
    mossy_material->set_compiled_shader_id(mossy_shader);
    materials["MOSSY_MATERIAL"] = std::move(mossy_material);
    
    auto pavement_material = DCraft::MaterialBuilder::create_lambert("Pavement");
    pavement_material->set_texture("assets/textures/rocky_terrain.jpg", DCraft::TextureType::DIFFUSE);
    pavement_material->set_uv_tiling(50.0f, 50.0f);
    uint32_t pavement_shader = shader_manager.get_shader_for_material(*pavement_material);
    pavement_material->set_compiled_shader_id(pavement_shader);
    materials["PAVEMENT_MATERIAL"] = std::move(pavement_material);

    
    auto grass_trans_material = DCraft::MaterialBuilder::create_lambert("Pavement");
    grass_trans_material->set_texture("assets/textures/transparent/grass.png", DCraft::TextureType::DIFFUSE);
    grass_trans_material->set_transparency(0.99f);
    uint32_t grass_shader = shader_manager.get_shader_for_material(*grass_trans_material);
    grass_trans_material->set_compiled_shader_id(grass_shader);
    materials["GRASS_MATERIAL"] = std::move(grass_trans_material);

    
    auto miquel_material = DCraft::MaterialBuilder::create_lambert("Miquel");
    miquel_material->set_texture("assets/textures/miquel.jpg", DCraft::TextureType::DIFFUSE);
    uint32_t miquel_shader = shader_manager.get_shader_for_material(*miquel_material);
    miquel_material->set_compiled_shader_id(miquel_shader);
    materials["MIQUEL_MATERIAL"] = std::move(miquel_material);
    
    auto denzel_material = DCraft::MaterialBuilder::create_lambert("Eljto");
    denzel_material->set_texture("assets/textures/eltjo.jpg", DCraft::TextureType::DIFFUSE);
    uint32_t denzel_shader = shader_manager.get_shader_for_material(*denzel_material);
    denzel_material->set_compiled_shader_id(denzel_shader);
    materials["ELTJO_MATERIAL"] = std::move(denzel_material);
    
    // Create Phong materials using MaterialBuilder
    // auto plastic_green_material = DCraft::MaterialBuilder::create_phong("PlasticGreen");
    // plastic_green_material->set_texture("assets/textures/plastic_green.jpg", DCraft::TextureType::DIFFUSE);
    // plastic_green_material->set_specular_color(glm::vec3(0.8f));
    // plastic_green_material->set_shininess(64.0f);
    // uint32_t green_shader = shader_manager.get_shader_for_material(*plastic_green_material);
    // plastic_green_material->set_compiled_shader_id(green_shader);
    // materials["PLASTIC_GREEN_MATERIAL"] = std::move(plastic_green_material);
    
    return materials;
}

DCraft::Scene *load_scene(DCraft::SceneManager &scene_manager, DCraft::WindowInfo window, DCraft::ShaderManager& shader_manager) {
    DCraft::Scene *scene = scene_manager.create_scene("Test Scene");
    scene->set_position(0.0f, 0.0f, 0.0f);

    // Materials with proper shader assignment
    MaterialMap materials = load_material_map(shader_manager);

    // Setup lighting
    auto* sun = new DCraft::DirectionalLight("Sun");
    sun->set_direction(glm::normalize(glm::vec3(0.2f, -0.8f, 0.1f)));
    sun->set_position(glm::vec3(20.0f, 30.0f, 20.0f));
    sun->set_color(glm::vec3(1.0f, 0.95f, 0.9f));
    sun->set_intensity(2.5f);
    scene->add(sun);

    // Cameras 
    auto *main_camera = scene->create_camera<DCraft::PerspectiveCamera>(
        "Main Camera", 70.0f, window.aspect_ratio, 0.1f, 400.0f);
    main_camera->set_position(0.0f, 1.0f, 12.0f);
    scene->add(main_camera);
    scene->set_active_camera(main_camera);

    auto* skybox = new DCraft::Skybox();
    skybox->set_cubemap_faces({
        "assets/skybox/east.png",   // +X
        "assets/skybox/west.png",    // -X
        "assets/skybox/up.png",     // +Y
        "assets/skybox/down.png",  // -Y
        "assets/skybox/north.png",   // +Z
        "assets/skybox/south.png"     // -Z
    });
    scene->set_skybox(skybox);

    auto *drone_camera = scene->create_camera<DCraft::PerspectiveCamera>(
        "Drone Camera", 70.0f, window.aspect_ratio, 0.1f, 400.0f);
    drone_camera->set_position(10.0f, 25.0f, 10.0f);
    drone_camera->set_target(0.0f, 0.0f, 0.0f);
    scene->add(drone_camera);

    auto *floor = new DCraft::Quad("Floor Quad");
    floor->set_position(0, 0, 0);
    floor->set_scale(glm::vec3(100.0f, 100.0f, 100.0f));
    floor->set_rotation(glm::vec3(-90.0f, 0.0f, 0.0f));
    floor->set_material(materials["PAVEMENT_MATERIAL"].release()); 
    scene->add(floor);

    auto *displayCase = new DisplayCase();
    scene->add(displayCase);

    auto *main_camera_visual = new DCraft::Cube("Main Camera Visual");
    main_camera_visual->set_name("Main Camera Visual");
    main_camera_visual->set_scale(glm::vec3(0.0f));
    main_camera_visual->set_position(main_camera->get_position());
    main_camera_visual->set_material(materials["MIQUEL_MATERIAL"].release()); 
    scene->add(main_camera_visual);

    auto *drone_camera_visual = new DCraft::Cube("Drone Camera Visual");
    drone_camera_visual->set_name("Drone Camera Visual");
    drone_camera_visual->set_scale(glm::vec3(0.3f));
    drone_camera_visual->set_position(drone_camera->get_position());
    drone_camera_visual->set_material(materials["ELTJO_MATERIAL"].release()); 

    auto *main_camera_direction = new DCraft::Cube("Main Camera Direction");
    main_camera_direction->set_name("Main Camera Direction");
    main_camera_direction->set_scale(glm::vec3(0.05f, 0.05f, 0.5f));
    main_camera_direction->set_position(0.0f, 0.0f, 1.0f);
    main_camera_direction->set_material(materials["PLASTIC_GREEN_MATERIAL"].release()); 
    main_camera_visual->add(main_camera_direction);

    auto *drone_camera_direction = new DCraft::Cube("Drone Camera Direction");
    drone_camera_direction->set_name("Drone Camera Direction");
    drone_camera_direction->set_scale(glm::vec3(0.05f, 0.05f, 0.5f));
    drone_camera_direction->set_position(0.0f, 0.0f, 1.0f);
    drone_camera_visual->add(drone_camera_direction);
    drone_camera_direction->set_material(materials["PLASTIC_GREEN_MATERIAL"].release());
    scene->add(drone_camera_visual);

    return scene;
}