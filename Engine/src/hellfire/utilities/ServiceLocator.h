//
// Created by denzel on 18/09/2025.
//

#pragma once
#include <typeindex>
#include <unordered_map>


namespace hellfire {
    class Renderer;
    class ShaderManager;
    class InputManager;

    class ServiceLocator {
    private:
        static std::unordered_map<std::type_index, void *> services_;

    public:
        template<typename T>
        static void register_service(T *service) {
            services_[std::type_index(typeid(T))] = service;
        }

        template<typename T>
        static T *get_service() {
            auto it = services_.find(std::type_index(typeid(T)));
            if (it != services_.end()) {
                return static_cast<T *>(it->second);
            }
            return nullptr;
        }
    };
}
