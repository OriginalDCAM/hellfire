//
// Created by denzel on 17/08/2025.
//

#include "Utils/AsteroidBeltFactory.h"

#include <random>

#include "assimp/contrib/poly2tri/poly2tri/common/utils.h"
#include "DCraft/Addons/Asset.h"
#include "DCraft/Components/InstancedRenderableComponent.h"
#include "DCraft/Components/RenderableComponent.h"

hellfire::Entity * AsteroidBeltFactory::create_asteroid_belt(const size_t quantity) {
    // Load all asteroid types
    const std::vector materials = {
        create_rocky_instanced_material(),
        create_metallic_instanced_material(), 
        create_icy_instanced_material(),
        create_crystal_instanced_material(),
        create_dusty_instanced_material()
    };
    
    auto asteroid_normal = hellfire::Asset::load("assets/models/asteroid_normal.obj");
    auto asteroid_metal_big = hellfire::Asset::load("assets/models/asteroid_metal_big.obj");
    auto asteroid_rocky = hellfire::Asset::load("assets/models/asteroid_rocky.obj");
    auto asteroid_crystal = hellfire::Asset::load("assets/models/asteroid_crystal.obj");
    auto asteroid_less_rocky = hellfire::Asset::load("assets/models/asteroid_less_rocky.obj");

    // Helper function to extract mesh
    auto extract_mesh = [](hellfire::Entity* entity) -> std::shared_ptr<hellfire::Mesh> {
        std::shared_ptr<hellfire::Mesh> mesh = nullptr;
        std::function<void(hellfire::Entity *)> find_mesh = [&](hellfire::Entity *e) {
            if (mesh) return;
            auto *renderable = e->get_component<hellfire::RenderableComponent>();
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
    
    auto *asteroid_belt = new hellfire::Entity("Asteroid Belt");
        const size_t total_asteroids = quantity;
    size_t asteroids_per_type = total_asteroids / asteroid_meshes.size();
    
    // Create separate child entity for each asteroid type
    for (size_t i = 0; i < asteroid_meshes.size(); ++i) {
        std::string type_name = "Asteroid Type " + std::to_string(i);
        auto *type_entity = new hellfire::Entity(type_name);
        
        // Add instanced component to child entity
        auto *instanced_comp = type_entity->add_component<hellfire::InstancedRenderableComponent>(
            asteroid_meshes[i], asteroids_per_type
        );
        instanced_comp->set_material(materials[i]);
        
        // Generate instances for this type
        std::vector<hellfire::InstancedRenderableComponent::InstanceData> asteroids = 
            generate_asteroid_belt_data(asteroids_per_type);
        instanced_comp->set_instances(asteroids);
        
        // Add child to main belt entity
        asteroid_belt->add(type_entity);
    }

    return asteroid_belt;
}

std::vector<hellfire::InstancedRenderableComponent::InstanceData> AsteroidBeltFactory::generate_asteroid_belt_data(
    const size_t quantity) {
    std::vector<hellfire::InstancedRenderableComponent::InstanceData> asteroids;

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


std::shared_ptr<hellfire::Material> AsteroidBeltFactory::create_rocky_instanced_material() {
    auto material = hellfire::MaterialBuilder::create_custom(
        "Rocky Instanced Material",
        "assets/shaders/instanced.vert",
        "assets/shaders/instanced.frag"
    );
    
    material->set_texture("assets/textures/planets/surfaces/moon.jpg", hellfire::TextureType::DIFFUSE);
    material->set_property("baseColor", glm::vec3(0.4f, 0.35f, 0.3f));   
    material->set_property("roughness", 0.8f);                         
    material->set_property("metallic", 0.0f);                          
    material->set_property("emissive", glm::vec3(0.0f, 0.0f, 0.0f));
    return material;
}

std::shared_ptr<hellfire::Material> AsteroidBeltFactory::create_metallic_instanced_material() {
    auto material = hellfire::MaterialBuilder::create_custom(
        "Metallic Instanced Material", 
        "assets/shaders/instanced.vert",
        "assets/shaders/instanced.frag"
    );

    
    material->set_texture("assets/textures/planets/surfaces/moon.jpg", hellfire::TextureType::DIFFUSE);
    material->set_property("baseColor", glm::vec3(0.3f, 0.3f, 0.35f));   
    material->set_property("roughness", 0.2f);                         
    material->set_property("metallic", 0.9f);                            
    material->set_property("emissive", glm::vec3(0.0f, 0.0f, 0.0f));
    return material;
}

std::shared_ptr<hellfire::Material> AsteroidBeltFactory::create_icy_instanced_material() {
    auto material = hellfire::MaterialBuilder::create_custom(
        "Icy Instanced Material",
        "assets/shaders/instanced.vert", 
        "assets/shaders/instanced.frag"
    );

    
    material->set_texture("assets/textures/planets/surfaces/moon.jpg", hellfire::TextureType::DIFFUSE);
    material->set_property("baseColor", glm::vec3(0.7f, 0.8f, 0.9f));    
    material->set_property("roughness", 0.1f);                          
    material->set_property("metallic", 0.0f);                           
    material->set_property("emissive", glm::vec3(0.0f, 0.0f, 0.0f));
    material->set_property("uTransparency", 0.8f);
    return material;
}

std::shared_ptr<hellfire::Material> AsteroidBeltFactory::create_crystal_instanced_material() {
    auto material = hellfire::MaterialBuilder::create_custom(
        "Crystal Instanced Material",
        "assets/shaders/instanced.vert",
        "assets/shaders/instanced.frag"  
    );

    
    material->set_texture("assets/textures/planets/surfaces/moon.jpg", hellfire::TextureType::DIFFUSE);
    material->set_property("baseColor", glm::vec3(0.6f, 0.3f, 0.8f));   
    material->set_property("roughness", 0.0f);                          
    material->set_property("metallic", 0.3f);                           
    material->set_property("emissive", glm::vec3(0.1f, 0.05f, 0.15f));   
    return material;
}

std::shared_ptr<hellfire::Material> AsteroidBeltFactory::create_dusty_instanced_material() {
    auto material = hellfire::MaterialBuilder::create_custom(
        "Dusty Instanced Material",
        "assets/shaders/instanced.vert",
        "assets/shaders/instanced.frag"
    );

    material->set_texture("assets/textures/planets/surfaces/moon.jpg", hellfire::TextureType::DIFFUSE);
    material->set_property("baseColor", glm::vec3(0.5f, 0.4f, 0.25f));   
    material->set_property("roughness", 0.9f);                          
    material->set_property("metallic", 0.0f);                           
    material->set_property("emissive", glm::vec3(0.0f, 0.0f, 0.0f));
    return material;
}
