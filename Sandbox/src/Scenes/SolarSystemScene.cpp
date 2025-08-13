#include "DCraft/Addons/Cameras.h"
#include "DCraft/Addons/ImportedModel3D.h"
#include "DCraft/Graphics/Lighting/DirectionalLight.h"
#include "Scenes/SolarSystemScene.h"

#include "DCraft/Graphics/Geometry/Sphere.h"
#include "DCraft/Graphics/Lighting/PointLight.h"
#include "Scripts/OrbitController.h"
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

    auto dynamic_earth_material = DCraft::MaterialBuilder::create_custom("Dynamic Earth Material", "assets/Shaders/Custom/earth_day_night.vert", "assets/Shaders/Custom/earth_day_night.frag");

    auto earth_cloud_material = DCraft::MaterialBuilder::create_lambert("Earth Cloud Material");
    earth_cloud_material->set_texture(planet_surface_dir + "earth_clouds.jpg", DCraft::TextureType::DIFFUSE);
    earth_cloud_material->set_transparency(0.50f);
    uint32_t earth_cloud_shader = shader_manager.get_shader_for_material(*earth_cloud_material);
    earth_cloud_material->set_compiled_shader_id(earth_cloud_shader);
    materials["EARTH_CLOUD_MATERIAL"] = std::move(earth_cloud_material);
   
    
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

DCraft::Entity* create_planet(const std::string& name, float scale, const glm::vec3& position, 
                             DCraft::Material* material, float orbit_radius = 0.0f, float orbit_speed = 0.0f) {
    auto* planet = DCraft::Sphere::create(name, glm::vec3(1.0f), 64, 64);
    planet->transform()->set_position(position.x, position.y, position.z);
    planet->transform()->set_scale(scale, scale, scale);
    planet->get_component<DCraft::RenderableComponent>()->set_material(material);
    
    // Add orbit animation if specified
    if (orbit_radius > 0.0f) {
        auto* orbit_script = planet->add_component<OrbitController>();
        orbit_script->set_orbit_radius(orbit_radius);
        orbit_script->set_orbit_speed(orbit_speed);
        orbit_script->set_center(glm::vec3(0.0f, 0.0f, 0.0f)); // Orbit around sun
    }
    
    return planet;
}
DCraft::Scene *load_solar_system_scene(DCraft::SceneManager &scene_manager, DCraft::WindowInfo window, DCraft::ShaderManager& shader_manager) {
    DCraft::Scene *scene = scene_manager.create_scene("Solar System Scene");
    MaterialMap materials = load_material_map(shader_manager);

    // Setup lighting - adjust direction for better planet illumination
    auto* sunlight = DCraft::PointLight::create("Sol Light", glm::vec3(0.0f), glm::vec3(1.0f), 1.2f, 350.0f);
    sunlight->get_component<DCraft::LightComponent>()->set_intensity(1.2f); 
    scene->add_entity(sunlight);

    auto *sun_visual = DCraft::Sphere::create("Sol", glm::vec3(1.0f), 64, 64);
    sun_visual->transform()->set_position(0, 0, 0);
    sun_visual->transform()->set_scale(25.0f); 
    sun_visual->get_component<DCraft::RenderableComponent>()->set_material(materials["SUN_MATERIAL"].release()); 
    scene->add_entity(sun_visual);

    auto *mercury_visual = create_planet("Mercury", 0.4f, glm::vec3(0,0,45), 
                                        materials["MERCURY_MATERIAL"].release(), 45.0f, 0.11f);
    scene->add_entity(mercury_visual);

    auto *venus_visual = create_planet("Venus", 0.9f, glm::vec3(0,0,65), 
                                      materials["VENUS_MATERIAL"].release(), 65.0f, 0.044f);
    scene->add_entity(venus_visual);

    auto *earth_visual = create_planet("Earth", 1.0f, glm::vec3(0,0,85), 
                                      materials["EARTH_MATERIAL"].release(), 85.0f, 0.028f);
    scene->add_entity(earth_visual);
    
    auto *earth_clouds = create_planet("Earth Cloud", 1.05f, glm::vec3(0,0,85), 
                                      materials["EARTH_CLOUD_MATERIAL"].release(), 85.0f, 0.028f);
    scene->add_entity(earth_clouds);

    auto *mars_visual = create_planet("Mars", 0.5f, glm::vec3(0,0,110), 
                                     materials["MARS_MATERIAL"].release(), 110.0f, 0.012f);
    scene->add_entity(mars_visual);

    auto *jupiter_visual = create_planet("Jupiter", 8.0f, glm::vec3(0,0,180), 
                                        materials["JUPITER_MATERIAL"].release(), 180.0f, 0.0002f);
    scene->add_entity(jupiter_visual);

    auto *saturn_visual = create_planet("Saturn", 7.5f, glm::vec3(0,0,240), 
                                       materials["SATURN_SURFACE_MATERIAL"].release(), 240.0f, 0.00008f);
    scene->add_entity(saturn_visual);

    auto* main_camera = DCraft::PerspectiveCamera::create("Main Camera", 70.0f, window.aspect_ratio, 0.1f, 1000.0f);
    
    main_camera->transform()->set_position(20.0f, 30.0f, 120.0f);
    main_camera->get_component<DCraft::CameraComponent>()->look_at(glm::vec3(0.0f, 0.0f, 0.0f));
    main_camera->add_component<PlayerController>(25.0f); 
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

// Enhanced planet creation with rotation
DCraft::Entity* create_planet_with_rotation(const std::string& name, float scale, const glm::vec3& position, 
                                           DCraft::Material* material, float orbit_radius = 0.0f, 
                                           float orbit_speed = 0.0f, float rotation_speed = 1.0f) {
    auto* planet = DCraft::Sphere::create(name);
    planet->transform()->set_position(position.x, position.y, position.z);
    planet->transform()->set_scale(scale, scale, scale);
    planet->get_component<DCraft::RenderableComponent>()->set_material(material);
    
    // Add orbit animation if specified
    if (orbit_radius > 0.0f) {
        auto* orbit_script = planet->add_component<OrbitController>();
        orbit_script->set_orbit_radius(orbit_radius);
        orbit_script->set_orbit_speed(orbit_speed);
        orbit_script->set_rotation_speed(rotation_speed); // Planet spinning on axis
        orbit_script->set_center(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    
    return planet;
}