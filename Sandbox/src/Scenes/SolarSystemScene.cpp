#include "DCraft/Addons/Cameras.h"
#include "DCraft/Addons/ImportedModel3D.h"
#include "DCraft/Graphics/Lighting/DirectionalLight.h"
#include "Scenes/SolarSystemScene.h"

#include "DCraft/Graphics/Geometry/Sphere.h"
#include "Scripts/PlayerController.h"

MaterialMap load_material_map(DCraft::ShaderManager& shader_manager) {
    MaterialMap materials;

    std::string planet_surface_dir = "assets/Textures/Planets/Surfaces/";
    
    auto sun_material = DCraft::MaterialBuilder::create_custom("Sun Material", "assets/Shaders/Custom/sun.vert", "assets/Shaders/Custom/sun.frag");
    sun_material->set_texture(planet_surface_dir + "sun.jpg", DCraft::TextureType::DIFFUSE);
    sun_material->set_uv_tiling(100.0f, 100.0f);
    uint32_t sun_shader = shader_manager.get_shader_for_material(*sun_material);
    sun_material->set_compiled_shader_id(sun_shader);
    materials["SUN_MATERIAL"] = std::move(sun_material);

    auto mercury_material = DCraft::MaterialBuilder::create_lambert("Mercury Material");
    mercury_material->set_texture(planet_surface_dir + "mercury.jpg", DCraft::TextureType::DIFFUSE);
    uint32_t mercury_shader = shader_manager.get_shader_for_material(*mercury_material);
    mercury_material->set_compiled_shader_id(mercury_shader);
    materials["MERCURY_MATERIAL"] = std::move(mercury_material);
    
    auto venus_material = DCraft::MaterialBuilder::create_lambert("Venus Material");
    venus_material->set_texture(planet_surface_dir + "venus_surface.jpg", DCraft::TextureType::DIFFUSE);
    uint32_t venus_shader = shader_manager.get_shader_for_material(*venus_material);
    venus_material->set_compiled_shader_id(venus_shader);
    materials["VENUS_MATERIAL"] = std::move(venus_material);

    auto earth_material = DCraft::MaterialBuilder::create_lambert("Earth Material");
    earth_material->set_texture(planet_surface_dir + "earth.jpg", DCraft::TextureType::DIFFUSE);
    uint32_t earth_shader = shader_manager.get_shader_for_material(*earth_material);
    earth_material->set_compiled_shader_id(earth_shader);
    materials["EARTH_MATERIAL"] = std::move(earth_material);

    
    auto mars_material = DCraft::MaterialBuilder::create_lambert("Mars Material");
    mars_material->set_texture(planet_surface_dir + "mars.jpg", DCraft::TextureType::DIFFUSE);
    uint32_t mars_shader = shader_manager.get_shader_for_material(*mars_material);
    mars_material->set_compiled_shader_id(mars_shader);
    materials["MARS_MATERIAL"] = std::move(mars_material);
    
    auto jupiter_material = DCraft::MaterialBuilder::create_lambert("Jupiter Material");
    jupiter_material->set_texture(planet_surface_dir + "jupiter.jpg", DCraft::TextureType::DIFFUSE);
    uint32_t jupiter_shader = shader_manager.get_shader_for_material(*jupiter_material);
    jupiter_material->set_compiled_shader_id(jupiter_shader);
    materials["JUPITER_MATERIAL"] = std::move(jupiter_material);

    auto saturn_surface_material = DCraft::MaterialBuilder::create_lambert("Saturn Surface Material");
    saturn_surface_material->set_texture(planet_surface_dir + "saturn_surface.jpg", DCraft::TextureType::DIFFUSE);
    uint32_t saturn_surface_shader = shader_manager.get_shader_for_material(*saturn_surface_material);
    saturn_surface_material->set_compiled_shader_id(saturn_surface_shader);
    materials["SATURN_SURFACE_MATERIAL"] = std::move(saturn_surface_material);
    
    return materials;
}

DCraft::Scene *load_solar_system_scene(DCraft::SceneManager &scene_manager, DCraft::WindowInfo window, DCraft::ShaderManager& shader_manager) {
    DCraft::Scene *scene = scene_manager.create_scene("Solar System Scene");
    MaterialMap materials = load_material_map(shader_manager);

    // Setup lighting
    auto* sunlight = DCraft::DirectionalLight::create("Sol Light", glm::vec3(1, -0.2, 1));
    scene->add_entity(sunlight);

    // Planets:
    auto *sun_visual = DCraft::Sphere::create("Sol");
    sun_visual->transform()->set_position(0, 0, 0);
    sun_visual->transform()->set_scale(109);
    sun_visual->get_component<DCraft::RenderableComponent>()->set_material(materials["SUN_MATERIAL"].release()); 
    scene->add_entity(sun_visual);

    auto *mercury_visual = DCraft::Sphere::create("Mercury");
    mercury_visual->transform()->set_position(0, 0, 150);
    mercury_visual->transform()->set_scale(0.33f);
    mercury_visual->get_component<DCraft::RenderableComponent>()->set_material(materials["MERCURY_MATERIAL"].release()); 
    scene->add_entity(mercury_visual);

    
    auto *venus_visual = DCraft::Sphere::create("Venus");
    venus_visual->transform()->set_position(0, 0, 175);
    venus_visual->transform()->set_scale(0.9f);
    venus_visual->get_component<DCraft::RenderableComponent>()->set_material(materials["VENUS_MATERIAL"].release()); 
    scene->add_entity(venus_visual);

    auto *earth_visual = DCraft::Sphere::create("Earth");
    earth_visual->transform()->set_position(0, 0, 200);
    earth_visual->transform()->set_rotation(180, 0, 0);
    earth_visual->transform()->set_scale(1);
    earth_visual->get_component<DCraft::RenderableComponent>()->set_material(materials["EARTH_MATERIAL"].release()); 
    scene->add_entity(earth_visual);

    
    auto *mars_visual = DCraft::Sphere::create("Mars");
    mars_visual->transform()->set_position(0, 0, 225);
    mars_visual->transform()->set_scale(0.5f);
    mars_visual->get_component<DCraft::RenderableComponent>()->set_material(materials["MARS_MATERIAL"].release()); 
    scene->add_entity(mars_visual);

    auto *jupiter_visual = DCraft::Sphere::create("Jupiter");
    jupiter_visual->transform()->set_position(0, 0, 325);
    jupiter_visual->transform()->set_scale(11);
    jupiter_visual->get_component<DCraft::RenderableComponent>()->set_material(materials["JUPITER_MATERIAL"].release());
    scene->add_entity(jupiter_visual);

    auto *saturn_visual = DCraft::Sphere::create("Saturn");
    saturn_visual->transform()->set_position(0, 0, 375);
    saturn_visual->transform()->set_scale(9);
    saturn_visual->get_component<DCraft::RenderableComponent>()->set_material(materials["SATURN_SURFACE_MATERIAL"].release());
    scene->add_entity(saturn_visual);

    // Cameras
    auto* main_camera = DCraft::PerspectiveCamera::create("Main Camera", 70.0f, window.aspect_ratio, 0.1f, 400.0f);
    main_camera->transform()->set_position(0.0f, 10.0f, 120.0f);
    main_camera->add_component<PlayerController>(10);
    scene->add_entity(main_camera);
    scene->set_active_camera(main_camera);

    auto* skybox = new DCraft::Skybox();
    skybox->set_cubemap_faces({
        "assets/Skybox/space_right.png",   // +X
        "assets/Skybox/space_left.png",    // -X
        "assets/Skybox/space_top.png",     // +Y
        "assets/Skybox/space_bottom.png",  // -Y
        "assets/Skybox/space_front.png",   // +Z
        "assets/Skybox/space_back.png"     // -Z
    });
    scene->set_skybox(skybox);



    return scene;
}