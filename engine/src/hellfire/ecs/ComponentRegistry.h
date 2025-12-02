//
// Created by denzel on 30/11/2025.
//

#pragma once
#include <functional>
#include <typeindex>

#include "Component.h"
#include "nlohmann/json.hpp"

namespace hellfire {
    class Entity;
    class Component;

    template<typename T>
    concept ComponentType = std::derived_from<T, Component>;

    class ComponentRegistry {
    public:
        using SerializeFn = std::function<nlohmann::json(const Component *)>;
        using DeserializeFn = std::function<void(Entity *, const nlohmann::json &)>;

        static ComponentRegistry &instance() {
            static ComponentRegistry registry;
            return registry;
        }

        template<ComponentType T>
        void register_component(const std::string &type_name, std::function<nlohmann::json(const T&)> serialize,
                                std::function<void(Entity *, const nlohmann::json &)> deserialize) {
            const auto type = std::type_index(typeid(T));
            type_names_[type] = type_name;
            name_to_type_.insert_or_assign(type_name, type);

            // Warp the typed function to match our storage type
            serializers_[type] = [serialize](const Component *c) -> nlohmann::json {
                return serialize(static_cast<const T &>(*c));
            };

            deserializers_[type_name] = std::move(deserialize);
        }

        nlohmann::json serialize(const Component *component) const {
            const auto type = std::type_index(typeid(*component));
            const auto it = serializers_.find(type);
            if (it == serializers_.end()) return nullptr;

            nlohmann::json j = it->second(component);
            j["_type"] = type_names_.at(type);
            return j;
        }

        void deserialize(Entity *entity, const nlohmann::json &j) const {
            const std::string type_name = j.at("_type");
            const auto it = deserializers_.find(type_name);
            if (it != deserializers_.end()) {
                it->second(entity, j);
            }
        }

        // Prevent copying
        ComponentRegistry(const ComponentRegistry&) = delete;
        ComponentRegistry& operator=(const ComponentRegistry&) = delete;
    private:
        ComponentRegistry() = default;

        std::unordered_map<std::type_index, std::string> type_names_;
        std::unordered_map<std::string, std::type_index> name_to_type_;
        std::unordered_map<std::type_index, SerializeFn> serializers_;
        std::unordered_map<std::string, DeserializeFn> deserializers_;
    };
}
