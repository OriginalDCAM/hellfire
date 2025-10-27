//
// Created by denzel on 17/08/2025.
//

#include "Utils/AsteroidBeltFactory.h"

#include <random>

#define _USE_MATH_DEFINES
#include <math.h>

#include "hellfire/assets/Asset.h"
#include "hellfire/ecs/InstancedRenderableComponent.h"
#include "hellfire/ecs/RenderableComponent.h"
#include "hellfire/ecs/components/MeshComponent.h"
#include "hellfire/scene/SceneManager.h"
#include "hellfire/utilities/ServiceLocator.h"

hellfire::EntityID AsteroidBeltFactory::create_asteroid_belt(hellfire::Scene *scene, const size_t quantity) {
    // Load all asteroid types
    std::vector<hellfire::EntityID> model_ids = {
        hellfire::Asset::load(scene, "assets/models/asteroid_normal.obj"),
        hellfire::Asset::load(scene, "assets/models/asteroid_metal_big.obj"),
        hellfire::Asset::load(scene, "assets/models/asteroid_rocky.obj"),
        hellfire::Asset::load(scene, "assets/models/asteroid_crystal.obj"),
        hellfire::Asset::load(scene, "assets/models/asteroid_less_rocky.obj")
    };

    // Helper function to extract mesh
    auto extract_mesh = [scene](hellfire::EntityID entity_id) -> std::shared_ptr<hellfire::Mesh> {
        std::shared_ptr<hellfire::Mesh> mesh = nullptr;
        std::function<void(hellfire::EntityID)> find_mesh = [&](hellfire::EntityID id) {
            if (mesh) return;

            hellfire::Entity* entity = scene->get_entity(id);
            if (!entity) return;

            auto* mesh_comp = entity->get_component<hellfire::MeshComponent>();
            if (mesh_comp && mesh_comp->has_mesh()) {
                mesh = mesh_comp->get_mesh();
            }

            // Search children
            for (hellfire::EntityID child_id : scene->get_children(id)) {
                find_mesh(child_id);
            }
        };

        find_mesh(entity_id);
        return mesh;
    };

    // Extract meshes
    std::vector<std::shared_ptr<hellfire::Mesh>> asteroid_meshes;
    for (hellfire::EntityID model_id : model_ids) {
        auto mesh = extract_mesh(model_id);
        if (mesh) {
            asteroid_meshes.push_back(mesh);
        }
        // Delete the loaded model entities - we only need their meshes
        scene->destroy_entity(model_id);
    }

    const std::vector<std::shared_ptr<hellfire::Material>> materials = {
        create_rocky_instanced_material(),
        create_metallic_instanced_material(),
        create_icy_instanced_material(),
        create_crystal_instanced_material(),
        create_dusty_instanced_material()
    };

    // Create main belt entity
    hellfire::EntityID belt_id = scene->create_entity("Asteroid Belt");

    const size_t asteroids_per_type = quantity / asteroid_meshes.size();

    // Create child entity for each asteroid type
    for (size_t i = 0; i < asteroid_meshes.size(); ++i) {
        std::string type_name = "Asteroid Type " + std::to_string(i);
        hellfire::EntityID type_id = scene->create_entity(type_name);
        hellfire::Entity* type_entity = scene->get_entity(type_id);

        // Add mesh component
        auto* mesh_comp = type_entity->add_component<hellfire::MeshComponent>();
        mesh_comp->set_mesh(asteroid_meshes[i]);

        // Add instanced renderable component
        auto* instanced_comp = type_entity->add_component<hellfire::InstancedRenderableComponent>(
            asteroid_meshes[i], asteroids_per_type
        );
        instanced_comp->set_material(materials[i % materials.size()]);

        // Generate instances
        std::vector<hellfire::InstancedRenderableComponent::InstanceData> asteroids =
            generate_asteroid_belt_data(asteroids_per_type);
        instanced_comp->set_instances(asteroids);

        // Parent to belt
        scene->set_parent(type_id, belt_id);
    }

    return belt_id;

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
