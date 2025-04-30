//
// Created by denzel on 11/04/2025.
//
#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "MaterialSerializer.h"
#include "DCraft/Addons/ModelLoader.h"

using json = nlohmann::json;
using namespace DCraft;

namespace DCraft {
    class Light;
    class DirectionalLight;
    class PerspectiveCamera;
    class SceneManager;

}
namespace DCraft {
    class ObjectDeserializer {
    public:
        ObjectDeserializer(SceneManager* scene_manager, Scene* scene_);

        Object3D* deserialize_object(const json& node_data);
    private:
        SceneManager* scene_manager_;
        Scene* scene_;
        std::unordered_map<std::string, std::function<Object3D*(const json&)>> object_factories_;
        
        void registerFactories();
    
        Object3D* create_shape(const json& data);
    
        Object3D* create_camera(const json& data);
    
        void set_camera_properties(PerspectiveCamera* camera, const json& data);
    
        Object3D* create_model(const json& data);

        void apply_saved_children_transforms(Object3D *parent, const json &children_data);

        Object3D* create_light(const json& data);
    
        Object3D* create_point_light(const json& data);
    
        Object3D* create_directional_light(const json& data);
    
        void set_light_color(Light* light, const json& data);
    
        void set_common_properties(Object3D* obj, const json& data);
    
        void set_transform(Object3D* obj, const json& data);
    
        void process_children(Object3D* parent, const json& data, const std::string& type);
    };
}