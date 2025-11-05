#include "../../../Engine/src/hellfire/scene/CameraFactory.h"
#include "hellfire/assets/Asset.h"
#include "hellfire/graphics/lighting/DirectionalLight.h"
#include "Scenes/SolarSystemScene.h"

#include "hellfire/ecs/InstancedRenderableComponent.h"
#include "hellfire/graphics/Geometry/Sphere.h"
#include "Scripts/OrbitController.h"
#include "Scripts/PlayerController.h"
#include "Utils/AsteroidBeltFactory.h"

MaterialMap load_material_map() {
    MaterialMap materials;

    std::string planet_surface_dir = "assets/textures/planets/surfaces/";

    auto sun_material = hellfire::MaterialBuilder::create_custom("Sun Material", "assets/shaders/custom/sun.vert",
                                                                 "assets/shaders/custom/sun.frag");
    sun_material->set_texture(planet_surface_dir + "sun.jpg", hellfire::TextureType::DIFFUSE);
    sun_material->set_uv_tiling(100.0f, 100.0f);
    materials["SUN_MATERIAL"] = sun_material;

    auto mercury_material = hellfire::MaterialBuilder::create_lambert("Mercury Material");
    mercury_material->set_texture(planet_surface_dir + "mercury.jpg", hellfire::TextureType::DIFFUSE);
    materials["MERCURY_MATERIAL"] = mercury_material;

    auto venus_material = hellfire::MaterialBuilder::create_lambert("Venus Material");
    venus_material->set_texture(planet_surface_dir + "venus_surface.jpg", hellfire::TextureType::DIFFUSE);
    materials["VENUS_MATERIAL"] = venus_material;

    auto earth_material = hellfire::MaterialBuilder::create_lambert("Earth Material");
    earth_material->set_texture(planet_surface_dir + "earth_daymap.jpg", hellfire::TextureType::DIFFUSE);
    materials["EARTH_MATERIAL"] = earth_material;

    auto dynamic_earth_material = hellfire::MaterialBuilder::create_custom(
        "Dynamic Earth Material", "assets/shaders/custom/earth_day_night.vert",
        "assets/shaders/custom/earth_day_night.frag");
    dynamic_earth_material->add_texture(planet_surface_dir + "earth_daymap.jpg", "uDayTexture", 0);
    dynamic_earth_material->add_texture(planet_surface_dir + "earth_nightmap.jpg", "uNightTexture", 1);
    materials["DYNAMIC_EARTH_MATERIAL"] = dynamic_earth_material;


    auto earth_cloud_material = hellfire::MaterialBuilder::create_lambert("Earth Cloud Material");
    earth_cloud_material->set_texture(planet_surface_dir + "earth_clouds.png", hellfire::TextureType::DIFFUSE);
    earth_cloud_material->set_transparency(0.50f);
    earth_cloud_material->set_property("useLuminanceAsAlpha", true);
    materials["EARTH_CLOUD_MATERIAL"] = earth_cloud_material;

    auto moon_material = hellfire::MaterialBuilder::create_lambert("Moon Material");
    moon_material->set_texture(planet_surface_dir + "moon.jpg", hellfire::TextureType::DIFFUSE);
    materials["MOON_MATERIAL"] = moon_material;

    auto mars_material = hellfire::MaterialBuilder::create_lambert("Mars Material");
    mars_material->set_texture(planet_surface_dir + "mars.jpg", hellfire::TextureType::DIFFUSE);
    materials["MARS_MATERIAL"] = mars_material;

    auto instanced_asteroid_material = hellfire::MaterialBuilder::create_custom(
        "Instanced Asteroid Material",
        "assets/shaders/instanced.vert",
        "assets/shaders/instanced.frag"
    );
    instanced_asteroid_material->set_uv_tiling(glm::vec2(10.0f));
    instanced_asteroid_material->set_texture(planet_surface_dir + "moon.jpg", hellfire::TextureType::DIFFUSE);
    materials["ASTEROID_MATERIAL"] = instanced_asteroid_material;

    auto jupiter_material = hellfire::MaterialBuilder::create_lambert("Jupiter Material");
    jupiter_material->set_texture(planet_surface_dir + "jupiter.jpg", hellfire::TextureType::DIFFUSE);
    materials["JUPITER_MATERIAL"] = jupiter_material;

    auto saturn_surface_material = hellfire::MaterialBuilder::create_lambert("Saturn Surface Material");
    saturn_surface_material->set_texture(planet_surface_dir + "saturn_surface.jpg", hellfire::TextureType::DIFFUSE);
    materials["SATURN_SURFACE_MATERIAL"] = saturn_surface_material;

    return materials;
}

hellfire::Scene *load_solar_system_scene(const hellfire::AppInfo &window) {
    auto *scene = new hellfire::Scene("Solar System Scene");
    MaterialMap materials = load_material_map();

    hellfire::EntityID world_id = scene->create_entity("Solar System World");

    // Sun light
    hellfire::EntityID sunlight_id = hellfire::DirectionalLight::create(scene, "Sol Light");
    scene->set_parent(sunlight_id, world_id);

    // Sun visual
    hellfire::EntityID sun_id = hellfire::Sphere::create(scene, "Sol", {
                                                             .color = glm::vec3(1.0f),
                                                             .material = materials["SUN_MATERIAL"],
                                                             .scale = glm::vec3(25.0f),
                                                             .rings = 64,
                                                             .sectors = 64
                                                         });
    scene->set_parent(sun_id, world_id);

    // Helper for creating planets
    auto create_planet = [&](const std::string &name, float scale, const glm::vec3 &position,
                             std::shared_ptr<hellfire::Material> material,
                             float orbit_radius = 0.0f, float orbit_speed = 0.0f) -> hellfire::EntityID {
        hellfire::EntityID id = hellfire::Sphere::create(scene, name, {
                                                             .material = material,
                                                             .position = position,
                                                             .scale = glm::vec3(scale),
                                                             .rings = 64,
                                                             .sectors = 64
                                                         });

        if (orbit_radius > 0.0f) {
            hellfire::Entity *planet = scene->get_entity(id);
            auto *orbit = planet->add_component<OrbitController>();
            orbit->set_orbit_radius(orbit_radius);
            orbit->set_orbit_speed(orbit_speed);
            orbit->set_center(glm::vec3(0.0f));
        }

        return id;
    };

    // Create planets
    hellfire::EntityID mercury_id = create_planet("Mercury", 0.4f, glm::vec3(0, 0, 45),
                                                  materials["MERCURY_MATERIAL"], 45.0f, 0.11f);
    scene->set_parent(mercury_id, world_id);

    hellfire::EntityID venus_id = create_planet("Venus", 0.9f, glm::vec3(0, 0, 65),
                                                materials["VENUS_MATERIAL"], 65.0f, 0.044f);
    scene->set_parent(venus_id, world_id);

    hellfire::EntityID earth_id = create_planet("Earth", 1.0f, glm::vec3(0, 0, 0),
                                                materials["DYNAMIC_EARTH_MATERIAL"], 85.0f, 0.028f);
    scene->set_parent(earth_id, world_id);

    // Earth clouds
    hellfire::EntityID clouds_id = hellfire::Sphere::create(scene, "Earth Clouds", {
                                                                .material = materials["EARTH_CLOUD_MATERIAL"],
                                                                .scale = glm::vec3(1.01f),
                                                                .rings = 64,
                                                                .sectors = 64
                                                            });
    hellfire::Entity *clouds = scene->get_entity(clouds_id);
    auto *clouds_orbit = clouds->add_component<OrbitController>();
    clouds_orbit->set_orbit_radius(0.0f);
    clouds_orbit->set_rotation_speed(0.12f);
    scene->set_parent(clouds_id, earth_id);

    // Moon
    hellfire::EntityID moon_id = create_planet("Moon", 0.25f, glm::vec3(0, 0, 2),
                                               materials["MOON_MATERIAL"], 5.0f, 0.014f);
    scene->set_parent(moon_id, earth_id);

    hellfire::EntityID mars_id = create_planet("Mars", 0.5f, glm::vec3(0, 0, 110),
                                               materials["MARS_MATERIAL"], 110.0f, 0.012f);
    scene->set_parent(mars_id, world_id);

    // Asteroid belt
    hellfire::EntityID asteroid_belt_id = AsteroidBeltFactory::create_asteroid_belt(scene, 250);
    scene->set_parent(asteroid_belt_id, world_id);

    hellfire::EntityID jupiter_id = create_planet("Jupiter", 8.0f, glm::vec3(0, 0, 180),
                                                  materials["JUPITER_MATERIAL"], 180.0f, 0.023f);
    scene->set_parent(jupiter_id, world_id);

    hellfire::EntityID saturn_id = create_planet("Saturn", 7.5f, glm::vec3(0, 0, 240),
                                                 materials["SATURN_SURFACE_MATERIAL"], 240.0f, 0.009655f);
    scene->set_parent(saturn_id, world_id);

    // Camera
    hellfire::EntityID camera_id = hellfire::PerspectiveCamera::create(
        scene, "Main Camera", 70.0f, window.aspect_ratio, 0.1f, 1000.0f,
        glm::vec3(20.0f, 30.0f, 200.0f)
    );
    hellfire::Entity *camera = scene->get_entity(camera_id);
    camera->get_component<hellfire::CameraComponent>()->look_at(glm::vec3(0.0f));
    camera->add_component<PlayerController>(12.5f);
    scene->set_parent(camera_id, world_id);
    scene->set_active_camera(camera_id);

    // Skybox
    auto *skybox = new hellfire::Skybox();
    skybox->set_cubemap_faces({
        "assets/skyboxes/space_right.png",
        "assets/skyboxes/space_left.png",
        "assets/skyboxes/space_bottom.png",
        "assets/skyboxes/space_top.png",
        "assets/skyboxes/space_front.png",
        "assets/skyboxes/space_back.png"
    });
    scene->set_skybox(skybox);

    return scene;
}
