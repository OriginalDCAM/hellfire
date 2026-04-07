//
// Created by denzel on 21/08/2025.
//

#include "Scenes/SponzaScene.h"

#include "DCraft/Application.h"
#include "DCraft/Addons/Asset.h"
#include "DCraft/Addons/CameraFactory.h"
#include "DCraft/Graphics/Lighting/PointLight.h"
#include "DCraft/Structs/Scene.h"
#include "Scripts/PlayerController.h"

DCraft::Scene * load_sponza_scene(DCraft::SceneManager &scene_manager, DCraft::WindowInfo window,
                                  DCraft::ShaderManager &shader_manager) {
    auto* scene = new DCraft::Scene("Sponza Scene");
    DCraft::Entity* sponza_model = DCraft::Asset::load("assets/models/sponza/scene.gltf");
    sponza_model->transform()->set_scale(0.01f);
    scene->add_entity(sponza_model);
    
    auto *sunlight = DCraft::PointLight::create("Sol Light", glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(1.0f), 1.2f, 350.0f);
    sunlight->get_component<DCraft::LightComponent>()->set_intensity(1.2f);
    scene->add_entity(sunlight);

    auto *main_camera = DCraft::PerspectiveCamera::create("Main Camera", 70.0f, window.aspect_ratio, 0.1f, 1000.0f);

    main_camera->transform()->set_position(5.0f, 5.0f, 5.0f);
    main_camera->get_component<DCraft::CameraComponent>()->look_at(glm::vec3(0.0f, 0.0f, 0.0f));
    main_camera->add_component<PlayerController>(7.5f);
    scene->add_entity(main_camera);
    scene->set_active_camera(main_camera);

    return scene;
}
