//
// Created by denzel on 30/11/2025.
//

#pragma once
#include <functional>
#include <typeindex>

#include "Entity.h"
#include "hellfire/serialization/Serializer.h"
#include "nlohmann/json.hpp"

namespace hellfire {
    class Entity;
    class Component;

    class ComponentRegistry {
    public:
        using SerializeFn = std::function<nlohmann::json(const Entity&)>;
        using DeserializeFn = std::function<void(Entity&, const nlohmann::json &)>;
        using HasComponentFn = std::function<bool(const Entity&)>;

        static ComponentRegistry &instance() {
            static ComponentRegistry registry;
            return registry;
        }

        // Prevent copying
        ComponentRegistry(const ComponentRegistry&) = delete;
        ComponentRegistry& operator=(const ComponentRegistry&) = delete;

        
        template<ComponentType T>
        void register_component(const std::string &type_name) {
            const auto type = std::type_index(typeid(T));
            type_names_[type] = type_name;
            name_to_type_.insert_or_assign(type_name, type);

            has_component_[type] = [](const Entity& e) {
              return e.has_component<T>();
            };

            // Lambda method for returning the serialization method
            serializers_[type] = [](const Entity& e) {
                std::ostringstream stream;
                Serializer<T>::serialize(stream, e.get_component<T>());
                nlohmann::json j = nlohmann::json::parse(stream.str());
                return j;
            };

            deserializers_[type_name] = [](Entity& e, const nlohmann::json& j) {
                auto comp = e.add_component<T>();
                std::istringstream stream(j.dump());
                Serializer<T>::deserialize(stream, comp);
            };
        }

        nlohmann::json serialize_all_components(const Entity& entity) const {
            nlohmann::ordered_json components = nlohmann::ordered_json::array();

            for (const auto& [type, has_fn] : has_component_) {
                if (has_fn(entity)) {
                    nlohmann::ordered_json comp = serializers_.at(type)(entity);
                    comp["_type"] = type_names_.at(type);
                    components.push_back(comp);
                }
            }
            
            return components;
        }

        bool deserialize_component(Entity& entity, const nlohmann::json& comp_json) const {
            if (!comp_json.contains("_type")) return false;

            std::string type_name = comp_json.at("_type");
            const auto it = deserializers_.find(type_name);
            if (it == deserializers_.end()) return false;

            it->second(entity, comp_json);
            return true;
        }

        void deserialize_all_components(Entity& entity, const nlohmann::json& components) const {
            for (const auto& comp_json : components) {
                deserialize_component(entity, comp_json);
            }
        }

    private:
        ComponentRegistry() = default;

        std::unordered_map<std::type_index, std::string> type_names_;
        std::unordered_map<std::string, std::type_index> name_to_type_;
        std::unordered_map<std::type_index, HasComponentFn> has_component_;
        std::unordered_map<std::type_index, SerializeFn> serializers_;
        std::unordered_map<std::string, DeserializeFn> deserializers_;
    };
}
