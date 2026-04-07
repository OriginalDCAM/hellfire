//
// Created by denzel on 07/08/2025.
//

#pragma once
namespace DCraft {
    class Entity;

    class Component {
    public:
        virtual ~Component() = default;

        Entity* get_owner() const { return owner_; }

        // Lifecycle hooks
        virtual void on_added(Entity* owner) { owner_ = owner; }
        virtual void on_removed() { owner_ = nullptr; }
    private:
        Entity* owner_ = nullptr;
    
    };
} // namespace DCraft