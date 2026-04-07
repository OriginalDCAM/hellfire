//
// Created by denzel on 07/10/2025.
//

#include "hellfire/graphics/Skybox.h"
#include "hellfire/graphics/geometry/Cube.h"

void setup_default_scene_with_default_entities(hellfire::Scene *scene) {
    hellfire::EntityID camera_id = hellfire::PerspectiveCamera::create(
        scene, "Main Camera",
        45.0f, 16.0f / 9.0f, 0.1f, 400.0f,
        glm::vec3(0, 0, 10)
    );

    scene->set_active_camera(camera_id);

    hellfire::Cube::create(scene, "Cube", {.rotation = glm::vec3(-32.0f, 22.0f, 0.0f)});

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


