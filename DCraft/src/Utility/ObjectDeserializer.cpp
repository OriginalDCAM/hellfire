//
// Created by denzel on 11/04/2025.
//
#include "DCraft/Utility/ObjectDeserializer.h"

#include "DCraft/Application.h"
#include "DCraft/Addons/ImportedModel3D.h"
#include "DCraft/Addons/PerspectiveCamera.h"
#include "DCraft/Graphics/Lights/DirectionalLight.h"
#include "DCraft/Graphics/Lights/PointLight.h"
#include "DCraft/Graphics/Primitives/Cube.h"
#include "DCraft/Graphics/Primitives/Quad.h"
#include "DCraft/Graphics/Primitives/MeshRenderer.h"
#include "DCraft/Structs/Scene.h"

using namespace DCraft;

ObjectDeserializer::ObjectDeserializer(SceneManager *scene_manager, Scene* scene): scene_manager_(scene_manager), scene_(scene) {
    registerFactories();
}

Object3D *ObjectDeserializer::deserialize_object(const json &node_data) {
    if (!node_data.contains("type")) {
        std::cerr << "NODE::ERROR Missing type for object" << std::endl;
        return nullptr;
    }

    std::string type = node_data["type"];

    // Find the factory function for this type
    auto factory_it = object_factories_.find(type);
    if (factory_it == object_factories_.end()) {
        std::cerr << "NODE::ERROR Unknown node type: " << type << std::endl;
        return nullptr;
    }

    // Create the object
    Object3D *obj = factory_it->second(node_data);
    if (!obj) return nullptr;

    // Set common properties and process children
    set_common_properties(obj, node_data);
    process_children(obj, node_data, type);

    return obj;
}

void ObjectDeserializer::registerFactories() {
    // Factory functions for each type
    object_factories_["Shape3D"] = [this](const json &data) {
        return create_shape(data);
    };

    object_factories_["Camera"] = [this](const json &data) {
        return create_camera(data);
    };

    object_factories_["Model"] = [this](const json &data) {
        return create_model(data);
    };

    object_factories_["Light"] = [this](const json &data) {
        return create_light(data);
    };
}

Object3D *ObjectDeserializer::create_shape(const json &data) {
    if (data.contains("geometry")) {
        std::string geometry = data["geometry"];
        if (geometry == "cube") {
            return new Cube("Default cube");
        } else if (geometry == "plane") {
            return new Quad("Default plane");
        } else {
            std::cerr << "GEOMETRY::ERROR Unknown geometry type: " << geometry << std::endl;
            return nullptr;
        }
    } else {
        return new MeshRenderer();
    }
}

Object3D *ObjectDeserializer::create_camera(const json &data) {
    std::string camera_type = data.contains("camera_type") ? data["camera_type"] : "perspective";

    if (camera_type == "perspective") {
        // Extract camera properties
        std::string name = data.contains("name") ? data["name"].get<std::string>() : "Default Camera";
        float fov = data.contains("fov") ? data["fov"].get<float>() : 70.0f;
        float aspect = data.contains("aspect_ratio") ? data["aspect_ratio"].get<float>() : 1.0f;
        float near_plane = data.contains("near_plane") ? data["near_plane"].get<float>() : 0.1f;
        float far_plane = data.contains("far_plane") ? data["far_plane"].get<float>() : 400.0f;

        // Create the camera
        PerspectiveCamera *camera = scene_->create_camera<PerspectiveCamera>(
            name, fov, aspect, near_plane, far_plane);

        // Set specific camera properties
        set_camera_properties(camera, data);

        return camera;
    } else {
        std::cerr << "CAMERA::ERROR Unknown camera type: " << camera_type << std::endl;
        return nullptr;
    }
}

void ObjectDeserializer::set_camera_properties(PerspectiveCamera *camera, const json &data) {
    // Set target if available
    if (data.contains("target") && data["target"].is_array() && data["target"].size() == 3) {
        camera->set_target(
            data["target"][0],
            data["target"][1],
            data["target"][2]
        );
    }

    // Set other camera-specific properties
    if (data.contains("front") && data["front"].is_array() && data["front"].size() == 3) {
        glm::vec3 front(
            data["front"][0],
            data["front"][1],
            data["front"][2]
        );
        camera->set_front(front);
    }

    
    if (data.contains("right") && data["right"].is_array() && data["right"].size() == 3) {
        glm::vec3 right(
            data["right"][0],
            data["right"][1],
            data["right"][2]
        );
        camera->set_right(right);
    }

    if (data.contains("up") && data["up"].is_array() && data["up"].size() == 3) {
        glm::vec3 up(
            data["up"][0],
            data["up"][1],
            data["up"][2]
        );
        camera->set_up(up);
    }

    if (data.contains("yaw")) camera->set_yaw(data["yaw"]);
    if (data.contains("pitch")) camera->set_pitch(data["pitch"]);
    if (data.contains("movement_speed")) camera->set_movement_speed(data["movement_speed"]);
    if (data.contains("mouse_sensitivity")) camera->set_mouse_sensitivity(data["mouse_sensitivity"]);
}

Object3D *ObjectDeserializer::create_model(const json &data) {
    // Check if it's a model with a path
    if (data.contains("path") && !data["path"].get<std::string>().empty()) {
        std::string path = data["path"];
        
        // Create model
        Object3D* obj = Addons::ModelLoader::load(path, scene_, Application::get_instance().get_shader_manager());
        
        if (obj) {
            if (data.contains("name")) {
                obj->set_name(data["name"]);
            }
            
            // Set transform
            set_transform(obj, data);
            
            // Set material if present
            if (data.contains("material")) {
                Material* material = MaterialSerializer::deserialize_material(data["material"]);
                if (auto* model = dynamic_cast<ImportedModel3D*>(obj)) {
                    model->set_material(material);
                }
            }
            
            // Apply saved transforms to children if they exist
            if (data.contains("children") && data["children"].is_array()) {
                apply_saved_children_transforms(obj, data["children"]);
            }
        }
        
        return obj;
    } else {
        // Child model without path
        auto* obj = new ImportedModel3D();
        if (data.contains("name")) {
            obj->set_name(data["name"]);
        }
        return obj;
    }
}

void ObjectDeserializer::apply_saved_children_transforms(Object3D* parent, const json& children_data) {
    // Create a map of child names to their saved data
    std::unordered_map<std::string, const json*> child_data_map;
    
    for (const auto& child_data : children_data) {
        if (child_data.contains("name")) {
            std::string name = child_data["name"];
            child_data_map[name] = &child_data;
        }
    }
    
    // Apply transforms and materials to existing children by matching names
    for (auto* child : parent->get_children()) {
        auto it = child_data_map.find(child->get_name());
        if (it != child_data_map.end()) {
            const json& saved_data = *(it->second);
            
            // Override transform completely
            if (saved_data.contains("transform")) {
                // Clear existing transform first
                child->set_position(glm::vec3(0.0f));
                child->set_rotation(glm::vec3(0.0f));
                child->set_scale(glm::vec3(1.0f));
                
                // Apply saved transform
                set_transform(child, saved_data);
            }
            
            // Apply material if it's a shape or model
            if (auto* shape = dynamic_cast<MeshRenderer*>(child)) {
                if (saved_data.contains("material")) {
                    // Replace existing material completely
                    Material* material = MaterialSerializer::deserialize_material(saved_data["material"]);
                    if (material) {
                        shape->set_material(material);
                    }
                }
            }
            
            // Process nested children recursively
            if (saved_data.contains("children") && saved_data["children"].is_array()) {
                apply_saved_children_transforms(child, saved_data["children"]);
            }
        }
    }
}

Object3D *ObjectDeserializer::create_light(const json &data) {
    if (data.contains("light_type")) {
        std::string light_type = data["light_type"];

        if (light_type == "PointLight") {
            return create_point_light(data);
        } else if (light_type == "DirectionalLight") {
            return create_directional_light(data);
        } else {
            std::cerr << "LIGHT::ERROR Unknown light type: " << light_type << std::endl;
            return nullptr;
        }
    } else {
        // Generic Object3D
        return new Object3D();
    }
}

Object3D *ObjectDeserializer::create_point_light(const json &data) {
    PointLight *light = new PointLight("PointLight");

    // Set light properties
    if (data.contains("intensity")) light->set_intensity(data["intensity"]);
    if (data.contains("range")) light->set_range(data["range"]);
    if (data.contains("attenuation")) light->set_attenuation(data["attenuation"]);

    // Set color if present
    set_light_color(light, data);

    return light;
}

Object3D *ObjectDeserializer::create_directional_light(const json &data) {
    DirectionalLight *light = new DirectionalLight("DirectionalLight");

    // Set light properties
    if (data.contains("intensity")) light->set_intensity(data["intensity"]);

    // Set color if present
    set_light_color(light, data);

    return light;
}

void ObjectDeserializer::set_light_color(Light *light, const json &data) {
    if (data.contains("color") && data["color"].is_array() && data["color"].size() == 3) {
        glm::vec3 color(
            data["color"][0],
            data["color"][1],
            data["color"][2]
        );
        light->set_color(color);
    }
}

void ObjectDeserializer::set_common_properties(Object3D *obj, const json &data) {
    // Set name
    if (data.contains("name")) {
        obj->set_name(data["name"]);
    }

    // Set transform
    set_transform(obj, data);

    // Set material for Shape3D objects
    if (auto *shape = dynamic_cast<MeshRenderer *>(obj)) {
        if (data.contains("material")) {
            Material *material = MaterialSerializer::deserialize_material(data["material"]);
            if (material) {
                shape->set_material(material);
            }
        }
    }
}

void ObjectDeserializer::set_transform(Object3D *obj, const json &data) {
    if (data.contains("transform")) {
        const auto &transform = data["transform"];

        // Set position
        if (transform.contains("position") && transform["position"].is_array() && transform["position"].size() == 3) {
            glm::vec3 position(
                transform["position"][0],
                transform["position"][1],
                transform["position"][2]
            );
            obj->set_position(position);
        }

        // Set rotation
        if (transform.contains("rotation") && transform["rotation"].is_array() && transform["rotation"].size() == 3) {
            glm::vec3 rotation(
                transform["rotation"][0],
                transform["rotation"][1],
                transform["rotation"][2]
            );
            obj->set_rotation(rotation);
        }

        // Set scale
        if (transform.contains("scale") && transform["scale"].is_array() && transform["scale"].size() == 3) {
            glm::vec3 scale(
                transform["scale"][0],
                transform["scale"][1],
                transform["scale"][2]
            );
            obj->set_scale(scale);
        }
    }
}

void ObjectDeserializer::process_children(Object3D *parent, const json &data, const std::string &type) {
    if (data.contains("children") && data["children"].is_array()) {
        // Skip processing children for models that were loaded from files
        bool skipChildren = (type == "Model" && data.contains("path") && 
                            !data["path"].get<std::string>().empty());
        
        if (!skipChildren) {
            for (const auto& child_data : data["children"]) {
                Object3D* child = deserialize_object(child_data);
                if (child) {
                    parent->add(child);
                }
            }
        }
    }

}
