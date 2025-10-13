//
// Created by denzel on 07/10/2025.
//

#include "RotateScript.h"
#include "hellfire/graphics/Skybox.h"
#include "hellfire/graphics/geometry/Cube.h"
#include "hellfire/graphics/geometry/Sphere.h"
#include "hellfire/graphics/lighting/DirectionalLight.h"

void setup_default_scene_with_default_entities(hellfire::Scene *scene) {
    hellfire::EntityID camera_id = hellfire::PerspectiveCamera::create(
        scene, "Main Camera",
        90.0f, 16.0f / 9.0f, 0.1f, 400.0f,
        glm::vec3(0, 0, 10)
    );

        hellfire::EntityID sunlight_id = hellfire::DirectionalLight::create(scene, "Sol Light", glm::vec3(-0.22f, 1.0f, 0.22f), glm::vec3(0.0f, 0.4f, 0.5f), 1.0f);

    scene->set_active_camera(camera_id);
    for (int i = 0; i < 1; i++) {
        auto entity_id = hellfire::Cube::create(scene, "Cube_" + std::to_string(i), {.rotation = glm::vec3(-45.0f, 45.0f, 45.0f)});
        scene->get_entity(entity_id)->add_component<RotateScript>();

        if (i % 2 == 0) {
        auto child_entity_id = hellfire::Sphere::create(scene, "Sphere_Child_" + std::to_string(i), {.color = glm::vec3(0.5, 0.2, 0.7), .position = glm::vec3(-1.5f, -1.5f, -1.5f)});
            scene->set_parent(child_entity_id, entity_id);
        }
        
    }

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
}

void create_default_scene() {
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


