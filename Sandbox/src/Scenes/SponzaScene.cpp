//
// Created by denzel on 21/08/2025.
//

#include "Scenes/SponzaScene.h"

#include "DCraft/Application.h"
#include "DCraft/Addons/Asset.h"
#include "DCraft/Addons/CameraFactory.h"
#include "DCraft/Graphics/Geometry/Quad.h"
#include "DCraft/Graphics/Lighting/DirectionalLight.h"
#include "DCraft/Structs/Scene.h"
#include "DCraft/Structs/Skybox.h"
#include "Scripts/PlayerController.h"

hellfire::Scene *load_sponza_scene(const hellfire::AppInfo& window) {
    auto *scene = new hellfire::Scene("Sponza Scene");
    hellfire::Entity *spaceshuttle_model = hellfire::Asset::load(
        "assets/models/shuttle/orbiter_space_shuttle_ov-103_discovery.glb");
    spaceshuttle_model->transform()->set_scale(0.1f);
    scene->add_entity(spaceshuttle_model);

    
    hellfire::Entity *spaceshuttle_model_1 = hellfire::Asset::load(
        "assets/models/shuttle/orbiter_space_shuttle_ov-103_discovery.glb");
    spaceshuttle_model_1->transform()->set_scale(0.1f);
    spaceshuttle_model_1->transform()->set_position(0,20,0);
    scene->add_entity(spaceshuttle_model_1);

    hellfire::Entity *spaceshuttle_model_2 = hellfire::Asset::load(
        "assets/models/shuttle/orbiter_space_shuttle_ov-103_discovery.glb");
    spaceshuttle_model_2->transform()->set_scale(0.1f);
    spaceshuttle_model_2->transform()->set_position(10,10,0);
    scene->add_entity(spaceshuttle_model_2);

    // hellfire::Entity *backpack_model = hellfire::Asset::load("assets/models/backpack/survival_guitar_backpack.glb");
    // backpack_model->transform()->set_position(30, 1, 30);
    // backpack_model->transform()->set_scale(0.1f);
    // scene->add_entity(backpack_model);

    // hellfire::Entity *character_model = hellfire::Asset::load("assets/models/guy/scene.gltf");
    // character_model->transform()->set_position(0.9, 0, 0);
    // character_model->transform()->set_scale(0.1f);
    // character_model->transform()->set_rotation(-90, 0, 0);
    // scene->add_entity(character_model);


    // hellfire::Entity *city_model = hellfire::Asset::load("assets/models/city/city.glb");
    // city_model->transform()->set_position(0, 0, 0);
    // city_model->transform()->set_rotation(-90, 0, 0);
    // city_model->transform()->set_scale(0.01);
    // scene->add_entity(city_model);

    // hellfire::Entity* water_plane = hellfire::Quad::create("Water Level", glm::vec3(0, 0, 0.5));
    // water_plane->transform()->set_scale(100);
    // water_plane->transform()->set_position(0, -7, 0);
    // scene->add_entity(water_plane);

    auto *sunlight = hellfire::DirectionalLight::create("Sol Light", glm::vec3(-0.22f, -1.0f, 0.0f), glm::vec3(1.0f),
                                                      1.2f);
    sunlight->get_component<hellfire::LightComponent>()->set_intensity(1.2f);
    scene->add_entity(sunlight);

    auto *main_camera = hellfire::PerspectiveCamera::create("Main Camera", 70.0f, window.aspect_ratio, 0.1f, 1000.0f);

    main_camera->transform()->set_position(0.0f, 0.0f, 0.0f);
    main_camera->add_component<PlayerController>(3.0f);
    scene->add_entity(main_camera);
    scene->set_active_camera(main_camera);

    auto *skybox = new hellfire::Skybox();
    skybox->set_cubemap_faces({
        "assets/skyboxes/px.png", // +X
        "assets/skyboxes/nx.png", // -X
        "assets/skyboxes/ny.png", // +Y
        "assets/skyboxes/py.png", // -Y
        "assets/skyboxes/pz.png", // +Z
        "assets/skyboxes/nz.png" // -Z
    });

    scene->set_skybox(skybox);

    return scene;
}
