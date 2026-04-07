//
// Created by denzel on 21/08/2025.
//

#include "Scenes/SponzaScene.h"

#include "DCraft/Application.h"
#include "DCraft/Addons/Asset.h"
#include "DCraft/Addons/CameraFactory.h"
#include "DCraft/Graphics/Geometry/Quad.h"
#include "DCraft/Graphics/Lighting/DirectionalLight.h"
#include "DCraft/Graphics/Lighting/PointLight.h"
#include "DCraft/Structs/Scene.h"
#include "DCraft/Structs/Skybox.h"
#include "Scripts/PlayerController.h"

DCraft::Scene * load_sponza_scene(DCraft::SceneManager &scene_manager, DCraft::WindowInfo window,
                                  DCraft::ShaderManager &shader_manager) {
    auto* scene = new DCraft::Scene("Sponza Scene");
    DCraft::Entity* sponza_model = DCraft::Asset::load("assets/models/shuttle/Orbiter_Space_Shuttle_OV-103_Discovery-150k-4096.gltf");
    sponza_model->transform()->set_scale(0.1f);
    scene->add_entity(sponza_model);

    
    DCraft::Entity* carrier_model = DCraft::Asset::load("assets/models/carrier/carrier.obj");
    carrier_model->transform()->set_position(0, -2.15, 0);
    carrier_model->transform()->set_scale(0.1f);
    scene->add_entity(carrier_model);

    // DCraft::Entity* water_plane = DCraft::Quad::create("Water Level", glm::vec3(0, 0, 0.5));
    // water_plane->transform()->set_scale(100);
    // water_plane->transform()->set_position(0, -7, 0);
    // scene->add_entity(water_plane);
    
    auto *sunlight = DCraft::DirectionalLight::create("Sol Light", glm::vec3(-0.22f, -1.0f, 0.0f), glm::vec3(1.0f), 1.2f);
    sunlight->get_component<DCraft::LightComponent>()->set_intensity(1.2f);
    scene->add_entity(sunlight);

    auto *main_camera = DCraft::PerspectiveCamera::create("Main Camera", 70.0f, window.aspect_ratio, 0.1f, 1000.0f);

    main_camera->transform()->set_position(5.0f, 5.0f, 5.0f);
    main_camera->get_component<DCraft::CameraComponent>()->look_at(glm::vec3(0.0f, 0.0f, 0.0f));
    main_camera->add_component<PlayerController>(6.0f);
    scene->add_entity(main_camera);
    scene->set_active_camera(main_camera);

    auto *skybox = new DCraft::Skybox();
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
