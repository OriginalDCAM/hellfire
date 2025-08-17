#include "DCraft/Addons/Cameras.h"
#include "DCraft/Addons/Asset.h"
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

    std::string planet_surface_dir = "assets/textures/planets/surfaces/";

    auto sun_material = DCraft::MaterialBuilder::create_custom("Sun Material", "assets/shaders/custom/sun.vert",
                                                               "assets/shaders/custom/sun.frag");
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
        "Dynamic Earth Material", "assets/shaders/custom/earth_day_night.vert",
        "assets/shaders/custom/earth_day_night.frag");
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
        "assets/shaders/instanced.vert",
        "assets/shaders/instanced.frag"
    );
    instanced_asteroid_material->set_uv_tiling(glm::vec2(10.0f));
    instanced_asteroid_material->set_texture(planet_surface_dir + "moon.jpg", DCraft::TextureType::DIFFUSE);
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
    std::uniform_real_distribution radius_dist(120.0f, 160.0f); // Between Mars and Jupiter
    std::uniform_real_distribution height_dist(-2.0f, 2.0f); // Vertical spread
    std::uniform_real_distribution scale_dist(0.01f, 0.3f); // Asteroid sizes
    std::uniform_real_distribution rotation_dist(0.0f, 360.0f); // Random rotations
    std::uniform_real_distribution color_variation(0.7f, 1.0f); // Grayish colors

    const size_t asteroid_count = quantity;
    asteroids.reserve(asteroid_count);

    for (size_t i = 0; i < asteroid_count; ++i) {
        // Random orbital position
        const float angle = angle_dist(gen);
        const float radius = radius_dist(gen);
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

        float brightness = color_variation(gen);
        glm::vec3 color(brightness * 0.8f, brightness * 0.7f, brightness * 0.6f); // Rocky colors

        // Create instance data
        asteroids.emplace_back(transform, color, scale);
    }

    return asteroids;
}
DCraft::Material* create_rocky_instanced_material() {
    auto material = DCraft::MaterialBuilder::create_custom(
        "Rocky Instanced Material",
        "assets/shaders/instanced.vert",
        "assets/shaders/instanced.frag"
    );
    
    material->set_texture("assets/textures/planets/surfaces/moon.jpg", DCraft::TextureType::DIFFUSE);
    material->set_property("baseColor", glm::vec3(0.4f, 0.35f, 0.3f));    // Gray-brown
    material->set_property("roughness", 0.8f);                           // Rough surface
    material->set_property("metallic", 0.0f);                            // Non-metallic
    material->set_property("emissive", glm::vec3(0.0f, 0.0f, 0.0f));
    return material.release();
}

DCraft::Material* create_metallic_instanced_material() {
    auto material = DCraft::MaterialBuilder::create_custom(
        "Metallic Instanced Material", 
        "assets/shaders/instanced.vert",
        "assets/shaders/instanced.frag"
    );

    
    material->set_texture("assets/textures/planets/surfaces/moon.jpg", DCraft::TextureType::DIFFUSE);
    material->set_property("baseColor", glm::vec3(0.3f, 0.3f, 0.35f));    // Dark gray
    material->set_property("roughness", 0.2f);                           // Smooth/shiny
    material->set_property("metallic", 0.9f);                            // Very metallic
    material->set_property("emissive", glm::vec3(0.0f, 0.0f, 0.0f));
    return material.release();
}

DCraft::Material* create_icy_instanced_material() {
    auto material = DCraft::MaterialBuilder::create_custom(
        "Icy Instanced Material",
        "assets/shaders/instanced.vert", 
        "assets/shaders/instanced.frag"
    );

    
    material->set_texture("assets/textures/planets/surfaces/moon.jpg", DCraft::TextureType::DIFFUSE);
    material->set_property("baseColor", glm::vec3(0.7f, 0.8f, 0.9f));     // Blue-white
    material->set_property("roughness", 0.1f);                           // Very smooth
    material->set_property("metallic", 0.0f);                            // Non-metallic
    material->set_property("emissive", glm::vec3(0.0f, 0.0f, 0.0f));
    material->set_property("transparency", 0.8f);
    return material.release();
}

DCraft::Material* create_crystal_instanced_material() {
    auto material = DCraft::MaterialBuilder::create_custom(
        "Crystal Instanced Material",
        "assets/shaders/instanced.vert",
        "assets/shaders/instanced.frag"  
    );

    
    material->set_texture("assets/textures/planets/surfaces/moon.jpg", DCraft::TextureType::DIFFUSE);
    material->set_property("baseColor", glm::vec3(0.6f, 0.3f, 0.8f));     // Purple
    material->set_property("roughness", 0.0f);                           // Mirror-like
    material->set_property("metallic", 0.3f);                            // Semi-metallic
    material->set_property("emissive", glm::vec3(0.1f, 0.05f, 0.15f));   // Slight glow
    return material.release();
}

DCraft::Material* create_dusty_instanced_material() {
    auto material = DCraft::MaterialBuilder::create_custom(
        "Dusty Instanced Material",
        "assets/shaders/instanced.vert",
        "assets/shaders/instanced.frag"
    );

    material->set_texture("assets/textures/planets/surfaces/moon.jpg", DCraft::TextureType::DIFFUSE);
    material->set_property("baseColor", glm::vec3(0.5f, 0.4f, 0.25f));    // Light brown
    material->set_property("roughness", 0.9f);                           // Very rough
    material->set_property("metallic", 0.0f);                            // Non-metallic
    material->set_property("emissive", glm::vec3(0.0f, 0.0f, 0.0f));
    return material.release();
}

DCraft::Entity *create_asteroid_belt() {
    // Load all asteroid types
    const std::vector materials = {
        create_rocky_instanced_material(),
        create_metallic_instanced_material(), 
        create_icy_instanced_material(),
        create_crystal_instanced_material(),
        create_dusty_instanced_material()
    };
    
    auto asteroid_normal = DCraft::Asset::load("assets/models/asteroid_normal.obj");
    auto asteroid_metal_big = DCraft::Asset::load("assets/models/asteroid_metal_big.obj");
    auto asteroid_rocky = DCraft::Asset::load("assets/models/asteroid_rocky.obj");
    auto asteroid_crystal = DCraft::Asset::load("assets/models/asteroid_crystal.obj");
    auto asteroid_less_rocky = DCraft::Asset::load("assets/models/asteroid_less_rocky.obj");

    // Helper function to extract mesh
    auto extract_mesh = [](DCraft::Entity* entity) -> std::shared_ptr<DCraft::Mesh> {
        std::shared_ptr<DCraft::Mesh> mesh = nullptr;
        std::function<void(DCraft::Entity *)> find_mesh = [&](DCraft::Entity *e) {
            if (mesh) return;
            auto *renderable = e->get_component<DCraft::RenderableComponent>();
            if (renderable && renderable->has_mesh()) {
                mesh = renderable->get_mesh_shared();
                return;
            }
            for (auto *child: e->get_children()) {
                find_mesh(child);
            }
        };
        find_mesh(entity);
        return mesh;
    };

    // Extract all meshes
    std::vector asteroid_meshes = {
        extract_mesh(asteroid_normal),
        extract_mesh(asteroid_metal_big), 
        extract_mesh(asteroid_rocky),
        extract_mesh(asteroid_crystal),
        extract_mesh(asteroid_less_rocky)
    };
    
    auto *asteroid_belt = new DCraft::Entity("Asteroid Belt");
        constexpr size_t total_asteroids = 25000;
    size_t asteroids_per_type = total_asteroids / asteroid_meshes.size();
    
    // Create separate child entity for each asteroid type
    for (size_t i = 0; i < asteroid_meshes.size(); ++i) {
        std::string type_name = "Asteroid Type " + std::to_string(i);
        auto *type_entity = new DCraft::Entity(type_name);
        
        // Add instanced component to THIS child entity
        auto *instanced_comp = type_entity->add_component<DCraft::InstancedRenderableComponent>(
            asteroid_meshes[i], asteroids_per_type
        );
        instanced_comp->set_material(materials[i]);
        
        // Generate instances for this type
        std::vector<DCraft::InstancedRenderableComponent::InstanceData> asteroids = 
            generate_asteroid_belt_data(asteroids_per_type);
        instanced_comp->set_instances(asteroids);
        
        // Add child to main belt entity
        asteroid_belt->add(type_entity);
    }

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
    auto *asteroid_belt_visual = create_asteroid_belt();
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
