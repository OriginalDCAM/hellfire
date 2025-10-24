//
// Created by denzel on 21/08/2025.
//

#include "Scenes/SponzaScene.h"

#include "hellfire-core.h"
#include "Scripts/PlayerController.h"

hellfire::Scene *load_sponza_scene(const hellfire::AppInfo& window) {
    const auto scene = new hellfire::Scene("Sponza Scene");
    hellfire::EntityID spaceshuttle_model_id = hellfire::Asset::load(scene, 
        "assets/models/shuttle/orbiter_space_shuttle_ov-103_discovery.glb");
    auto* spaceshuttle_one = scene->get_entity(spaceshuttle_model_id);
    spaceshuttle_one->transform()->set_scale(0.1f);

    
    hellfire::EntityID spaceshuttle_model_id_two = hellfire::Asset::load(scene, 
        "assets/models/shuttle/orbiter_space_shuttle_ov-103_discovery.glb");
    auto* spaceshuttle_two = scene->get_entity(spaceshuttle_model_id_two);
    spaceshuttle_two->transform()->set_scale(0.1f);
    spaceshuttle_two->transform()->set_position(0,20,0);
    
    
    hellfire::EntityID city_model_id = hellfire::Asset::load(scene, "assets/models/sponza-palace/source/scene.glb");
    auto* city_model = scene->get_entity(city_model_id);
    city_model->transform()->set_position(0, 0, 0);
    city_model->transform()->set_scale(0.01);

    hellfire::EntityID sunlight_id = hellfire::DirectionalLight::create(scene, "Sol Light", glm::vec3(-0.22f, -1.0f, 0.0f), glm::vec3(1.0f), 0.6f);
    
    // Camera
    hellfire::EntityID camera_id = hellfire::PerspectiveCamera::create(
        scene, "Main Camera", 70.0f, window.aspect_ratio, 0.1f, 1000.0f,
        glm::vec3(10.0f, 5.0f, 0.0f)
    );
    hellfire::Entity *camera = scene->get_entity(camera_id);
    camera->add_component<SceneCameraScript>(10.0f);
    scene->set_default_camera(camera_id);

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
