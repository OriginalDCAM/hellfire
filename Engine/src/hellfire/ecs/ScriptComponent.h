//
// Created by denzel on 10/08/2025.
//

#pragma once
#include <string>
#include <unordered_map>

#include "Entity.h"
#include "Component.h"
#include "glm/detail/type_vec.hpp"

#define SCRIPT_CLASS(ClassName) \
    public: \
    const char* get_class_name() const override { return #ClassName; } \
    static const char* static_class_name() { return #ClassName; }

// Property macro that registers the variable for serialization
#define SCRIPT_VAR(Type, Name, Default) \
Type Name = Default

// Helper macro for registration of variables
#define REGISTER_VAR(Name, Type) \
register_property(#Name, &Name, PropertyType::Type)

namespace hellfire {
    class TransformComponent;

    class ScriptComponent : public Component {
    public:
        enum class PropertyType {
            FLOAT, INT, BOOL, STRING, VEC2, VEC3, VEC4, COLOR3, COLOR4, TEXTURE, MATERIAL, ARRAY
        };

        struct PropertyInfo {
            std::string name;
            PropertyType type;
            void *data_ptr; // Pointer to actual member variable

            // Helper to get/set value through pointer
            template<typename T>
            T &get() { return *static_cast<T *>(data_ptr); }
        };

    protected:
        std::vector<PropertyInfo> properties_;

        // Register a property for editor/serialization
        template<typename T>
        void register_property(const std::string &name, T *ptr, PropertyType type) {
            properties_.push_back({name, type, static_cast<void *>(ptr)});
        }

        bool enabled_ = true;
    public:
        ScriptComponent() = default;

        virtual ~ScriptComponent() = default;

        const std::vector<PropertyInfo> &get_properties() const { return properties_; }

        // Virtual lifecycle methods
        virtual void on_init() {
        }

        virtual void on_update(float delta_time) {
        }

        virtual void on_remove() {
        }

        virtual void on_event(const std::string &event_name, void *data = nullptr) {
        }

        // Called by the entity system
        void init() { on_init(); }
        void update(float delta_time) { on_update(delta_time); }
        void remove() { on_remove(); }

        void trigger_event(const std::string &event_name, void *data = nullptr) { on_event(event_name, data); }

        void set_enabled(const bool enabled) { enabled_ = enabled; }

        bool is_enabled() const { return enabled_; }

        // Utility methods for common operations
        TransformComponent *get_transform() const;

        // Helper method to check if the owner has a specific component
        template<typename T>
        bool has_component() const {
            return get_owner().has_component<T>();
        }

        template<typename T>
        T *get_component() const {
            return get_owner().get_component<T>();
        }

        virtual const char *get_class_name() const {
            return {};
        }
    };
}
