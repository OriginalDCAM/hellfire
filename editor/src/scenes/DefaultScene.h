//
// Created by denzel on 07/10/2025.
//
#pragma once

#include "RotateScript.h"
#include "hellfire/graphics/Skybox.h"
#include "hellfire/graphics/geometry/Cube.h"
#include "hellfire/graphics/geometry/Sphere.h"
#include "hellfire/graphics/lighting/DirectionalLight.h"
#include "hellfire/utilities/FileDialog.h"

inline void setup_default_scene_with_default_entities(hellfire::Scene *scene) {
    auto light_id = hellfire::DirectionalLight::create(
        scene, "Sol Light", glm::vec3(-0.22f, 1.0f, 0.22f), glm::vec3(0.0f, 0.4f, 0.5f), 1.0f);
    const auto light_entity = scene->get_entity(light_id);
    light_entity->transform()->set_rotation(-35, -28, 0);
    


    const hellfire::EntityID cube_id = hellfire::Cube::create(scene, "Rotating cube", {});
    const auto cube_entity = scene->get_entity(cube_id);
    cube_entity->add_component<RotateScript>();

    auto *skybox = new hellfire::Skybox();
    skybox->set_cubemap_faces({
        "assets/skyboxes/px.png", // +X
        "assets/skyboxes/nx.png", // -X
        "assets/skyboxes/py.png", // +Y
        "assets/skyboxes/ny.png", // -Y
        "assets/skyboxes/pz.png", // +Z
        "assets/skyboxes/nz.png" // -Z
    });

    scene->environment()->set_skybox(*skybox);
}

inline void create_default_scene() {
    const auto sm = hellfire::ServiceLocator::get_service<hellfire::SceneManager>();
    hellfire::Utility::FileFilter scene_ext_filter = {"Hellfire Scene", "*.hfscene"};
    
    std::string scene_name; // Name gets passed as a reference to the save_file method of the file dialog
    const std::string save_path = hellfire::Utility::FileDialog::save_file(scene_name, "Untitled", {scene_ext_filter});
    
    if (!scene_name.empty()) {
        const auto new_scene = sm->create_scene(scene_name);
    
        setup_default_scene_with_default_entities(new_scene);
    
        sm->save_scene(save_path, new_scene);
        sm->set_active_scene(new_scene);
    }
}
