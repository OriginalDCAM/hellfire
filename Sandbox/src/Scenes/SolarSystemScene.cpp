#include "DCraft/Addons/Cameras.h"
#include "DCraft/Addons/ImportedModel3D.h"
#include "DCraft/Graphics/Lighting/DirectionalLight.h"
#include "Scenes/SolarSystemScene.h"

#include <random>

#include "assimp/contrib/poly2tri/poly2tri/common/utils.h"
#include "DCraft/Components/InstancedRenderableComponent.h"
#include "DCraft/Graphics/Geometry/Sphere.h"
#include "DCraft/Graphics/Lighting/PointLight.h"
#include "Scripts/OrbitController.h"
#include "Scripts/PlayerController.h"

MaterialMap load_material_map(DCraft::ShaderManager &shader_manager) {
    MaterialMap materials;

    std::string planet_surface_dir = "assets/Textures/Planets/Surfaces/";

    auto sun_material = DCraft::MaterialBuilder::create_custom("Sun Material", "assets/Shaders/Custom/sun.vert",
                                                               "assets/Shaders/Custom/sun.frag");
    sun_material->set_texture(planet_surface_dir + "sun.jpg", DCraft::TextureType::DIFFUSE);
    sun_material->set_uv_tiling(100.0f, 100.0f);
    materials["SUN_MATERIAL"] = std::move(sun_material);

    auto mercury_material = DCraft::MaterialBuilder::create_lambert("Mercury Material");
    mercury_material->set_texture(planet_surface_dir + "mercury.jpg", DCraft::TextureType::DIFFUSE);
    materials["MERCURY_MATERIAL"] = std::move(mercury_material);

    auto venus_material = DCraft::MaterialBuilder::create_lambert("Venus Material");
    venus_material->set_texture(planet_surface_dir + "venus_surface.jpg", DCraft::TextureType::DIFFUSE);
    materials["VENUS_MATERIAL"] = std::move(venus_material);

    auto earth_material = DCraft::MaterialBuilder::create_lambert("Earth Material");
    earth_material->set_texture(planet_surface_dir + "earth_daymap.jpg", DCraft::TextureType::DIFFUSE);
    materials["EARTH_MATERIAL"] = std::move(earth_material);

    auto dynamic_earth_material = DCraft::MaterialBuilder::create_custom(
        "Dynamic Earth Material", "assets/Shaders/Custom/earth_day_night.vert",
        "assets/Shaders/Custom/earth_day_night.frag");
    dynamic_earth_material->add_texture(planet_surface_dir + "earth_daymap.jpg", "uDayTexture", 0);
    dynamic_earth_material->add_texture(planet_surface_dir + "earth_nightmap.jpg", "uNightTexture", 1);
    materials["DYNAMIC_EARTH_MATERIAL"] = std::move(dynamic_earth_material);


    auto earth_cloud_material = DCraft::MaterialBuilder::create_lambert("Earth Cloud Material");
    earth_cloud_material->set_texture(planet_surface_dir + "earth_clouds.png", DCraft::TextureType::DIFFUSE);
    earth_cloud_material->set_transparency(0.75f);
    materials["EARTH_CLOUD_MATERIAL"] = std::move(earth_cloud_material);

    auto moon_material = DCraft::MaterialBuilder::create_lambert("Moon Material");
    moon_material->set_texture(planet_surface_dir + "moon.jpg", DCraft::TextureType::DIFFUSE);
    materials["MOON_MATERIAL"] = std::move(moon_material);

    auto mars_material = DCraft::MaterialBuilder::create_lambert("Mars Material");
    mars_material->set_texture(planet_surface_dir + "mars.jpg", DCraft::TextureType::DIFFUSE);
    materials["MARS_MATERIAL"] = std::move(mars_material);

    auto instanced_asteroid_material = DCraft::MaterialBuilder::create_custom(
    "Instanced Asteroid Material", 
    "assets/Shaders/instanced.vert",
    "assets/Shaders/instanced.frag"
);
    instanced_asteroid_material->set_texture(planet_surface_dir +"moon.jpg", DCraft::TextureType::DIFFUSE);
    materials["ASTEROID_MATERIAL"] = std::move(instanced_asteroid_material);

    auto jupiter_material = DCraft::MaterialBuilder::create_lambert("Jupiter Material");
    jupiter_material->set_texture(planet_surface_dir + "jupiter.jpg", DCraft::TextureType::DIFFUSE);
    materials["JUPITER_MATERIAL"] = std::move(jupiter_material);

    auto saturn_surface_material = DCraft::MaterialBuilder::create_lambert("Saturn Surface Material");
    saturn_surface_material->set_texture(planet_surface_dir + "saturn_surface.jpg", DCraft::TextureType::DIFFUSE);
    materials["SATURN_SURFACE_MATERIAL"] = std::move(saturn_surface_material);

    return materials;
}

DCraft::Entity *create_planet(const std::string &name, float scale, const glm::vec3 &position,
                              DCraft::Material *material, float orbit_radius = 0.0f, float orbit_speed = 0.0f,
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

DCraft::Entity *create_moon(const std::string &name, float scale, glm::vec3 center, DCraft::Material *material,
                            float orbit_radius = 0.0f, float orbit_speed = 0.0f) {
    return create_planet(name, scale, glm::vec3(0.0f), material, orbit_radius, orbit_speed, center);
}

DCraft::Entity *create_planet_with_rotation(const std::string &name, float scale, const glm::vec3 &position,
                                            DCraft::Material *material, float orbit_radius = 0.0f,
                                            float orbit_speed = 0.0f, float rotation_speed = 1.0f) {
    auto *planet = DCraft::Sphere::create(name);
    planet->transform()->set_position(position.x, position.y, position.z);
    planet->transform()->set_scale(scale, scale, scale);
    planet->get_component<DCraft::RenderableComponent>()->set_material(material);

    // Add orbit animation if specified
    if (orbit_radius > 0.0f) {
        auto *orbit_script = planet->add_component<OrbitController>();
        orbit_script->set_orbit_radius(orbit_radius);
        orbit_script->set_orbit_speed(orbit_speed);
        orbit_script->set_rotation_speed(rotation_speed); // Planet spinning on axis
        orbit_script->set_center(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    return planet;
}
std::vector<DCraft::InstancedRenderableComponent::InstanceData> generate_asteroid_belt_data(const size_t quantity) {
    std::vector<DCraft::InstancedRenderableComponent::InstanceData> asteroids;
    
    // Setup random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Define distributions for asteroid belt parameters
    std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> radius_dist(120.0f, 160.0f);  // Between Mars and Jupiter
    std::uniform_real_distribution<float> height_dist(-2.0f, 2.0f);     // Slight vertical spread
    std::uniform_real_distribution<float> scale_dist(0.01f, 0.3f);       // Asteroid sizes
    std::uniform_real_distribution<float> rotation_dist(0.0f, 360.0f);  // Random rotations
    std::uniform_real_distribution<float> color_variation(0.7f, 1.0f);  // Grayish colors
    
    const size_t asteroid_count = quantity;
    asteroids.reserve(asteroid_count);
    
    for (size_t i = 0; i < asteroid_count; ++i) {
        // Random orbital position
        float angle = angle_dist(gen);
        float radius = radius_dist(gen);
        float height = height_dist(gen);
        
        glm::vec3 position(
            radius * std::cos(angle),
            height,
            radius * std::sin(angle)
        );
        
        // Random rotations for each asteroid
        glm::vec3 rotation(
            rotation_dist(gen),
            rotation_dist(gen),
            rotation_dist(gen)
        );
        
        // Random scale
        float scale = scale_dist(gen);
        
        // Create transform matrix
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
        transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, glm::vec3(scale));
        
        // Random grayish color with slight variation
        float brightness = color_variation(gen);
        glm::vec3 color(brightness * 0.8f, brightness * 0.7f, brightness * 0.6f); // Rocky colors
        
        // Create instance data
        asteroids.emplace_back(transform, color, scale);
    }
    
    return asteroids;
}

DCraft::Entity *create_asteroid_belt(DCraft::Material* material) {
    auto asteroid = DCraft::Sphere::create("Asteroid", glm::vec3(1.0f), 8, 8);
    auto asteroid_mesh = asteroid->get_component<DCraft::RenderableComponent>()->get_mesh_shared();
    auto *asteroid_belt = new DCraft::Entity("Asteroid Belt");
    constexpr size_t asteroid_belt_quantity = 50000;
    auto *instanced_comp = asteroid_belt->add_component<DCraft::InstancedRenderableComponent>(asteroid_mesh
        , asteroid_belt_quantity); 
    instanced_comp->set_material(material);
    std::vector<DCraft::InstancedRenderableComponent::InstanceData> asteroids = generate_asteroid_belt_data(asteroid_belt_quantity);
    instanced_comp->set_instances(asteroids);

    return asteroid_belt;
}

DCraft::Scene *load_solar_system_scene(DCraft::SceneManager &scene_manager, DCraft::WindowInfo window,
                                       DCraft::ShaderManager &shader_manager) {
    DCraft::Scene *scene = scene_manager.create_scene("Solar System Scene");
    MaterialMap materials = load_material_map(shader_manager);

    auto *world = new DCraft::Entity("Solar System World");
    scene->add_entity(world);

    // Setup lighting
    auto *sunlight = DCraft::PointLight::create("Sol Light", glm::vec3(0.0f), glm::vec3(1.0f), 1.2f, 350.0f);
    sunlight->get_component<DCraft::LightComponent>()->set_intensity(1.2f);
    world->add(sunlight);

    auto *sun_visual = DCraft::Sphere::create("Sol", glm::vec3(1.0f), 64, 64);
    sun_visual->transform()->set_position(0, 0, 0);
    sun_visual->transform()->set_scale(25.0f);
    sun_visual->get_component<DCraft::RenderableComponent>()->set_material(materials["SUN_MATERIAL"].release());
    world->add(sun_visual);

    auto *mercury_visual = create_planet("Mercury", 0.4f, glm::vec3(0, 0, 45),
                                         materials["MERCURY_MATERIAL"].release(), 45.0f, 0.11f);
    world->add(mercury_visual);

    auto *venus_visual = create_planet("Venus", 0.9f, glm::vec3(0, 0, 65),
                                       materials["VENUS_MATERIAL"].release(), 65.0f, 0.044f);
    world->add(venus_visual);

    auto *earth_visual = create_planet("Earth", 1.0f, glm::vec3(0, 0, 85),
                                       materials["DYNAMIC_EARTH_MATERIAL"].release(), 85.0f, 0.028f);
    world->add(earth_visual);

    auto *earth_clouds = create_planet_with_rotation("Earth Clouds", 1.005f, glm::vec3(0, 0, 0),
                                                     materials["EARTH_CLOUD_MATERIAL"].release(), 0.0f, 0.0f, 0.1f);

    auto *moon_visual = create_planet("Moon", 0.25f, glm::vec3(0, 0, 2), materials["MOON_MATERIAL"].release(), 5.0f,
                                      0.014f);
    earth_visual->add(earth_clouds);
    earth_visual->add(moon_visual);

    auto *mars_visual = create_planet("Mars", 0.5f, glm::vec3(0, 0, 110),
                                      materials["MARS_MATERIAL"].release(), 110.0f, 0.012f);
    world->add(mars_visual);


    // Asteroid belt
    auto *asteroid_belt_visual = create_asteroid_belt(materials["ASTEROID_MATERIAL"].release());
    world->add(asteroid_belt_visual);

    auto *jupiter_visual = create_planet("Jupiter", 8.0f, glm::vec3(0, 0, 180),
                                         materials["JUPITER_MATERIAL"].release(), 180.0f, 0.0002f);
    world->add(jupiter_visual);

    auto *saturn_visual = create_planet("Saturn", 7.5f, glm::vec3(0, 0, 240),
                                        materials["SATURN_SURFACE_MATERIAL"].release(), 240.0f, 0.00008f);
    world->add(saturn_visual);

    auto *main_camera = DCraft::PerspectiveCamera::create("Main Camera", 70.0f, window.aspect_ratio, 0.1f, 1000.0f);

    main_camera->transform()->set_position(20.0f, 30.0f, 120.0f);
    main_camera->get_component<DCraft::CameraComponent>()->look_at(glm::vec3(0.0f, 0.0f, 0.0f));
    main_camera->add_component<PlayerController>(10.0f);
    world->add(main_camera);
    scene->set_active_camera(main_camera);

    auto *skybox = new DCraft::Skybox();
    skybox->set_cubemap_faces({
        "assets/Skybox/space_right.png", // +X
        "assets/Skybox/space_left.png", // -X
        "assets/Skybox/space_top.png", // +Y
        "assets/Skybox/space_bottom.png", // -Y
        "assets/Skybox/space_front.png", // +Z
        "assets/Skybox/space_back.png" // -Z
    });

    scene->set_skybox(skybox);

    return scene;
}
