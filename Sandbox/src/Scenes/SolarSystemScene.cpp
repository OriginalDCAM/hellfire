#include "DCraft/Addons/CameraFactory.h"
#include "DCraft/Addons/Asset.h"
#include "DCraft/Graphics/Lighting/DirectionalLight.h"
#include "Scenes/SolarSystemScene.h"

#include "DCraft/Components/InstancedRenderableComponent.h"
#include "DCraft/Graphics/Geometry/Sphere.h"
#include "DCraft/Graphics/Lighting/PointLight.h"
#include "Scripts/OrbitController.h"
#include "Scripts/PlayerController.h"
#include "Utils/AsteroidBeltFactory.h"

MaterialMap load_material_map() {
    MaterialMap materials;

    std::string planet_surface_dir = "assets/textures/planets/surfaces/";

    auto sun_material = DCraft::MaterialBuilder::create_custom("Sun Material", "assets/shaders/custom/sun.vert",
                                                               "assets/shaders/custom/sun.frag");
    sun_material->set_texture(planet_surface_dir + "sun.jpg", DCraft::TextureType::DIFFUSE);
    sun_material->set_uv_tiling(100.0f, 100.0f);
    materials["SUN_MATERIAL"] = sun_material;

    auto mercury_material = DCraft::MaterialBuilder::create_lambert("Mercury Material");
    mercury_material->set_texture(planet_surface_dir + "mercury.jpg", DCraft::TextureType::DIFFUSE);
    materials["MERCURY_MATERIAL"] = mercury_material;

    auto venus_material = DCraft::MaterialBuilder::create_lambert("Venus Material");
    venus_material->set_texture(planet_surface_dir + "venus_surface.jpg", DCraft::TextureType::DIFFUSE);
    materials["VENUS_MATERIAL"] = venus_material;

    auto earth_material = DCraft::MaterialBuilder::create_lambert("Earth Material");
    earth_material->set_texture(planet_surface_dir + "earth_daymap.jpg", DCraft::TextureType::DIFFUSE);
    materials["EARTH_MATERIAL"] = earth_material;

    auto dynamic_earth_material = DCraft::MaterialBuilder::create_custom(
        "Dynamic Earth Material", "assets/shaders/custom/earth_day_night.vert",
        "assets/shaders/custom/earth_day_night.frag");
    dynamic_earth_material->add_texture(planet_surface_dir + "earth_daymap.jpg", "uDayTexture", 0);
    dynamic_earth_material->add_texture(planet_surface_dir + "earth_nightmap.jpg", "uNightTexture", 1);
    materials["DYNAMIC_EARTH_MATERIAL"] = dynamic_earth_material;


    auto earth_cloud_material = DCraft::MaterialBuilder::create_lambert("Earth Cloud Material");
    earth_cloud_material->set_texture(planet_surface_dir + "earth_clouds.png", DCraft::TextureType::DIFFUSE);
    earth_cloud_material->set_transparency(0.50f);
    materials["EARTH_CLOUD_MATERIAL"] = earth_cloud_material;

    auto moon_material = DCraft::MaterialBuilder::create_lambert("Moon Material");
    moon_material->set_texture(planet_surface_dir + "moon.jpg", DCraft::TextureType::DIFFUSE);
    materials["MOON_MATERIAL"] = moon_material;

    auto mars_material = DCraft::MaterialBuilder::create_lambert("Mars Material");
    mars_material->set_texture(planet_surface_dir + "mars.jpg", DCraft::TextureType::DIFFUSE);
    materials["MARS_MATERIAL"] = mars_material;

    auto instanced_asteroid_material = DCraft::MaterialBuilder::create_custom(
        "Instanced Asteroid Material",
        "assets/shaders/instanced.vert",
        "assets/shaders/instanced.frag"
    );
    instanced_asteroid_material->set_uv_tiling(glm::vec2(10.0f));
    instanced_asteroid_material->set_texture(planet_surface_dir + "moon.jpg", DCraft::TextureType::DIFFUSE);
    materials["ASTEROID_MATERIAL"] = instanced_asteroid_material;

    auto jupiter_material = DCraft::MaterialBuilder::create_lambert("Jupiter Material");
    jupiter_material->set_texture(planet_surface_dir + "jupiter.jpg", DCraft::TextureType::DIFFUSE);
    materials["JUPITER_MATERIAL"] = jupiter_material;

    auto saturn_surface_material = DCraft::MaterialBuilder::create_lambert("Saturn Surface Material");
    saturn_surface_material->set_texture(planet_surface_dir + "saturn_surface.jpg", DCraft::TextureType::DIFFUSE);
    materials["SATURN_SURFACE_MATERIAL"] = saturn_surface_material;

    return materials;
}

DCraft::Entity *create_planet(const std::string &name, float scale, const glm::vec3 &position,
                              const std::shared_ptr<DCraft::Material> &material, float orbit_radius = 0.0f,
                              float orbit_speed = 0.0f,
                              const glm::vec3 &center = glm::vec3(0.0f)) {
    auto *planet = DCraft::Sphere::create(name, glm::vec3(1.0f), 64, 64);
    planet->transform()->set_position(position.x, position.y, position.z);
    planet->transform()->set_scale(1.0f * scale);
    planet->get_component<DCraft::RenderableComponent>()->set_material(material);

    if (orbit_radius > 0.0f) {
        auto *orbit_script = planet->add_component<OrbitController>();
        orbit_script->set_orbit_radius(orbit_radius);
        orbit_script->set_orbit_speed(orbit_speed);
        orbit_script->set_center(center);
    }

    return planet;
}

DCraft::Entity *create_planet_with_rotation(const std::string &name, float scale, const glm::vec3 &position,
                                            const std::shared_ptr<DCraft::Material> &material,
                                            float orbit_radius = 0.0f,
                                            float orbit_speed = 0.0f, float rotation_speed = 1.0f) {
    auto *planet = DCraft::Sphere::create(name);
    planet->transform()->set_position(position.x, position.y, position.z);
    planet->transform()->set_scale(scale, scale, scale);
    planet->get_component<DCraft::RenderableComponent>()->set_material(material);

    if (orbit_radius > 0.0f) {
        auto *orbit_script = planet->add_component<OrbitController>();
        orbit_script->set_orbit_radius(orbit_radius);
        orbit_script->set_orbit_speed(orbit_speed);
        orbit_script->set_rotation_speed(rotation_speed);
        orbit_script->set_center(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    return planet;
}

DCraft::Scene *load_solar_system_scene(DCraft::WindowInfo window) {
    const auto scene = new DCraft::Scene("Solar System Scene");
    MaterialMap materials = load_material_map();

    auto *world = new DCraft::Entity("Solar System World");
    scene->add_entity(world);

    // Setup lighting
    auto *sunlight = DCraft::PointLight::create("Sol Light", glm::vec3(0.0f), glm::vec3(1.0f), 1.2f, 350.0f);
    sunlight->get_component<DCraft::LightComponent>()->set_intensity(1.2f);
    world->add(sunlight);

    auto *sun_visual = DCraft::Sphere::create("Sol", glm::vec3(1.0f), 64, 64);
    sun_visual->transform()->set_position(0, 0, 0);
    sun_visual->transform()->set_scale(25.0f);
    sun_visual->get_component<DCraft::RenderableComponent>()->set_material(materials["SUN_MATERIAL"]);
    world->add(sun_visual);

    auto *mercury_visual = create_planet("Mercury", 0.4f, glm::vec3(0, 0, 45),
                                         materials["MERCURY_MATERIAL"], 45.0f, 0.11f);
    world->add(mercury_visual);

    auto *venus_visual = create_planet("Venus", 0.9f, glm::vec3(0, 0, 65),
                                       materials["VENUS_MATERIAL"], 65.0f, 0.044f);
    world->add(venus_visual);

    auto *earth_visual = create_planet("Earth", 1.0f, glm::vec3(0, 0, 85),
                                       materials["DYNAMIC_EARTH_MATERIAL"], 85.0f, 0.028f);
    world->add(earth_visual);

    auto *earth_clouds = create_planet_with_rotation("Earth Clouds", 1.01f, glm::vec3(0, 0, 0),
                                                     materials["EARTH_CLOUD_MATERIAL"], 0.0f, 0.0f, 0.12f);

    auto *moon_visual = create_planet("Moon", 0.25f, glm::vec3(0, 0, 2), materials["MOON_MATERIAL"], 5.0f,
                                      0.014f);
    earth_visual->add(earth_clouds);
    earth_visual->add(moon_visual);

    auto *mars_visual = create_planet("Mars", 0.5f, glm::vec3(0, 0, 110),
                                      materials["MARS_MATERIAL"], 110.0f, 0.012f);
    world->add(mars_visual);

    // Asteroid belt
    auto *asteroid_belt_visual = AsteroidBeltFactory::create_asteroid_belt(25000);
    world->add(asteroid_belt_visual);

    auto *jupiter_visual = create_planet("Jupiter", 8.0f, glm::vec3(0, 0, 180),
                                         materials["JUPITER_MATERIAL"], 180.0f, 0.0002f);
    world->add(jupiter_visual);

    auto *saturn_visual = create_planet("Saturn", 7.5f, glm::vec3(0, 0, 240),
                                        materials["SATURN_SURFACE_MATERIAL"], 240.0f, 0.00008f);
    world->add(saturn_visual);

    auto *main_camera = DCraft::PerspectiveCamera::create("Main Camera", 70.0f, window.aspect_ratio, 0.1f, 1000.0f);

    main_camera->transform()->set_position(20.0f, 30.0f, 200.0f);
    main_camera->get_component<DCraft::CameraComponent>()->look_at(glm::vec3(0.0f, 0.0f, 0.0f));
    main_camera->add_component<PlayerController>(7.5f);
    world->add(main_camera);
    scene->set_active_camera(main_camera);

    auto *skybox = new DCraft::Skybox();
    skybox->set_cubemap_faces({
        "assets/skyboxes/space_right.png", // +X
        "assets/skyboxes/space_left.png", // -X
        "assets/skyboxes/space_top.png", // +Y
        "assets/skyboxes/space_bottom.png", // -Y
        "assets/skyboxes/space_front.png", // +Z
        "assets/skyboxes/space_back.png" // -Z
    });

    scene->set_skybox(skybox);

    return scene;
}
