//
// Created by denzel on 07/04/2025.
//

#include "DCraft/Graphics/Lights/DirectionalLight.h"
#include "DCraft/Graphics/Lights/PointLight.h"
#include "DCraft/Graphics/Materials/LambertMaterial.h"
#include "DCraft/Graphics/Materials/PhongMaterial.h"
#include "DCraft/Graphics/Primitives/Cube.h"
#include "DCraft/Graphics/Primitives/Quad.h"
#include "Scenes/SandboxScene.h"

MaterialMap load_material_map() {
    // Create Lambert materials (diffuse only)
    auto* mossy_material = new DCraft::LambertMaterial("Mossy");
    mossy_material->set_texture("assets/textures/mossy_brick.jpg", DCraft::TextureType::DIFFUSE);
    mossy_material->set_ambient_color(glm::vec3(0.1f, 0.1f, 0.1f));
    mossy_material->set_diffuse_color(glm::vec3(0.8f, 0.8f, 0.8f));
    
    auto* pavement_material = new DCraft::LambertMaterial("Pavement");
    pavement_material->set_texture("assets/textures/brick_pavement.jpg", DCraft::TextureType::DIFFUSE);
    pavement_material->set_ambient_color(glm::vec3(0.1f, 0.1f, 0.1f));
    pavement_material->set_diffuse_color(glm::vec3(0.8f, 0.8f, 0.8f));
    
    auto* miquel_material = new DCraft::LambertMaterial("Miquel");
    miquel_material->set_texture("assets/textures/miquel.jpg", DCraft::TextureType::DIFFUSE);
    
    auto* denzel_material = new DCraft::LambertMaterial("Eljto");
    denzel_material->set_texture("assets/textures/eltjo.jpg", DCraft::TextureType::DIFFUSE);
    
    // Create Phong materials (diffuse + specular)
    auto* plastic_green_material = new DCraft::PhongMaterial("PlasticGreen");
    plastic_green_material->set_texture("assets/textures/plastic_green.jpg", DCraft::TextureType::DIFFUSE);
    plastic_green_material->set_ambient_color(glm::vec3(0.0f, 0.1f, 0.0f));
    plastic_green_material->set_diffuse_color(glm::vec3(0.0f, 0.8f, 0.0f));
    plastic_green_material->set_specular_color(glm::vec3(0.8f));
    plastic_green_material->set_shininess(64.0f);
    
    auto* plastic_blue_material = new DCraft::PhongMaterial("PlasticBlue");
    plastic_blue_material->set_texture("assets/textures/plastic_blue.jpg", DCraft::TextureType::DIFFUSE);
    plastic_blue_material->set_ambient_color(glm::vec3(0.0f, 0.0f, 1.0f));
    plastic_blue_material->set_diffuse_color(glm::vec3(0.0f, 0.0f, 0.8f));
    plastic_blue_material->set_specular_color(glm::vec3(0.9f));
    plastic_blue_material->set_shininess(128.0f);
    
    return {
        {"PLASTIC_BLUE_MATERIAL", plastic_blue_material}, 
        {"ELTJO_MATERIAL", denzel_material},
        {"PLASTIC_GREEN_MATERIAL", plastic_green_material}, 
        {"MIQUEL_MATERIAL", miquel_material},
        {"MOSSY_MATERIAL", mossy_material}, 
        {"PAVEMENT_MATERIAL", pavement_material}
    };
}


DCraft::Scene *load_scene(DCraft::SceneManager &scene_manager, DCraft::WindowInfo window) {
        DCraft::Scene *scene = scene_manager.create_scene("Test Scene");
    scene->set_position(0.0f, 0.0f, 0.0f);

    // Materials
    MaterialMap materials = load_material_map();

    // Setup lighting
    // Main directional light (sun)
    auto* sun = new DCraft::DirectionalLight("Sun");
    sun->set_direction(glm::vec3(-0.2f, -0.3f, -0.2f));
    sun->set_position(glm::vec3(20.0f, 30.0f, 20.0f));
    sun->set_color(glm::vec3(1.0f, 0.95f, 0.9f)); // Slightly warm sunlight
    sun->set_intensity(3.0f);

    // Visual for sunlight
    auto* sun_light_visual = new DCraft::Cube("Cube");
    sun_light_visual->set_name("Sun Light Visual");
    sun_light_visual->set_position(sun->get_position());
    sun_light_visual->set_material(materials["PLASTIC_BLUE_MATERIAL"]);
    sun_light_visual->add(sun);
    scene->add(sun_light_visual);
    
    // Blue point light
    auto* blue_light = new DCraft::PointLight("Blue Light");
    blue_light->set_color(glm::vec3(0.2f, 0.4f, 1.0f)); // Blue light
    blue_light->set_intensity(5.0f);
    blue_light->set_range(20.0f);
    blue_light->set_attenuation(1.0f);
    
    // Green point light
    auto* green_light = new DCraft::PointLight("Red Light");
    green_light->set_color(glm::vec3(0.2f, 1.0f, 0.1f)); // Red light
    green_light->set_intensity(1.5f);
    green_light->set_range(15.0f);
    green_light->set_attenuation(1.0f);
    
    // Visual indicator for blue light
    auto* blue_light_visual = new DCraft::Cube("Blue cube");
    blue_light_visual->set_position(-5.0f, 1.0f, -5.0f);
    blue_light_visual->set_scale(glm::vec3(0.2f));
    blue_light_visual->set_material(materials["PLASTIC_BLUE_MATERIAL"]);
    blue_light_visual->add(blue_light);
    scene->add(blue_light_visual);
    
    // Visual indicator for green light
    auto* green_light_visual = new DCraft::Cube("Green cube");
    green_light_visual->set_position(5.0f, 2.0f, 5.0f);
    green_light_visual->set_scale(glm::vec3(0.2f));
    green_light_visual->set_material(materials["PLASTIC_GREEN_MATERIAL"]);
    green_light_visual->add(green_light);
    scene->add(green_light_visual);

    // Main camera
    DCraft::PerspectiveCamera *main_camera = scene->create_camera<DCraft::PerspectiveCamera>(
        "Main Camera", 70.0f, window.aspect_ratio, 0.1f, 400.0f);
    main_camera->set_position(0.0f, 1.0f, 12.0f);
    scene->add(main_camera);
    scene->set_active_camera(main_camera);

    // drone camera
    DCraft::PerspectiveCamera *drone_camera = scene->create_camera<DCraft::PerspectiveCamera>(
        "Drone Camera", 70.0f, window.aspect_ratio, 0.1f, 400.0f);
    drone_camera->set_position(10.0f, 25.0f, 10.0f);
    drone_camera->set_target(0.0f, 0.0f, 0.0f);
    scene->add(drone_camera);
    
    auto *cube = new DCraft::Cube("Cool cube");
    cube->set_rotation(glm::vec3(90.0f, 0.0f, 0.0f));
    cube->set_scale(glm::vec3(1.0f, 1.0f, 1.0f));
    cube->set_position(0.0f, 4.0f, 0.0f);
    cube->set_material(materials["MOSSY_MATERIAL"]);
    scene->add(cube);

    // Plane primitive object
    auto *floor = new DCraft::Quad("Floor Quad");
    floor->set_position(0, 0, 0);
    floor->set_scale(glm::vec3(100.0f, 100.0f, 100.0f));
    floor->set_rotation(glm::vec3(-90.0f, 0.0f, 0.0f));
    floor->set_material(materials["PAVEMENT_MATERIAL"]);
    scene->add(floor);

    auto *main_camera_visual = new DCraft::Cube("Main Camera Visual");
    main_camera_visual->set_name("Main Camera Visual");
    main_camera_visual->set_scale(glm::vec3(0.0f));
    main_camera_visual->set_position(main_camera->get_position());
    main_camera_visual->set_material(materials["MIQUEL_MATERIAL"]);
    scene->add(main_camera_visual);

    // Visual indicator for drone camera position
    auto *drone_camera_visual = new DCraft::Cube("Drone Camera Visual");
    drone_camera_visual->set_name("Drone Camera Visual");
    drone_camera_visual->set_scale(glm::vec3(0.3f));
    drone_camera_visual->set_position(drone_camera->get_position());
    drone_camera_visual->set_material(materials["ELTJO_MATERIAL"]);

    auto *main_camera_direction = new DCraft::Cube("Main Camera Direction");
    main_camera_direction->set_name("Main Camera Direction");
    main_camera_direction->set_scale(glm::vec3(0.05f, 0.05f, 0.5f));
    main_camera_direction->set_position(0.0f, 0.0f, 1.0f);
    main_camera_direction->set_material(materials["PLASTIC_GREEN_MATERIAL"]);
    main_camera_visual->add(main_camera_direction);

    // Direction indicator for drone camera
    auto *drone_camera_direction = new DCraft::Cube("Drone Camera Direction");
    drone_camera_direction->set_name("Drone Camera Direction");
    drone_camera_direction->set_scale(glm::vec3(0.05f, 0.05f, 0.5f));
    drone_camera_direction->set_position(0.0f, 0.0f, 1.0f);
    drone_camera_visual->add(drone_camera_direction);
    drone_camera_direction->set_material(materials["PLASTIC_GREEN_MATERIAL"]);
    scene->add(drone_camera_visual);

    return scene;
}

