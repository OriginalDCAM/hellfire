#pragma once
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "DCraft/Structs/Transform3D.h"
#include "DCraft/Graphics/Mesh.h"

namespace DCraft {
    
    class alignas(16) Object3D {
    public:
        virtual ~Object3D() {
            for (auto *child: children_) {
                std::clog << "Deleting child named: " << child->get_name() << ", from the parent: " << get_name() << '\n';
                delete child;
            }
            children_.clear();
        }

        Object3D() : transform_(Transform3D()) {
        }

        Object3D(const std::string name) : transform_(Transform3D()) {
            set_name(name);
        }

        void set_name(std::string name) {
            name_ = std::move(name);
        }

        std::string &get_name() {
            return name_;
        }

        // Find objects
        Object3D *find_object_by_name(const std::string &name);

        // Mesh handling
        void add_mesh(const Mesh &mesh) {
            if (!mesh_) {
                mesh_ = std::make_unique<Mesh>(mesh);
            } else {
                *mesh_ = mesh;
            }
        }

        void match_orientation(const Object3D &other);

        glm::mat4 get_rotation_matrix() const { return transform_.get_rotation_matrix(); }

        void look_at(const glm::vec3& target, const glm::vec3& up);

        glm::vec3 get_default_front() const;

        Mesh *get_mesh() {
            if (!mesh_) {
                mesh_ = std::make_unique<Mesh>();
            }
            return mesh_.get();
        }

        bool has_mesh() const {
            return mesh_ != nullptr;
        }

        const std::vector<unsigned int> &get_indices() const {
            static const std::vector<unsigned int> empty_indices;
            return mesh_ ? mesh_->indices : empty_indices;
        }

        // Transform delegation methods
        const glm::vec3 &get_position() const { return transform_.get_position(); }

        const glm::vec3 get_world_position() const;

        void set_position(const glm::vec3 &new_position) {
            transform_.set_position(new_position);
        }

        void set_position(const float x, const float y, const float z) {
            transform_.set_position(x, y, z);
        }


        void set_scale(const glm::vec3 &new_scale) {
            transform_.set_scale(new_scale);
        }

        glm::vec3 get_current_scale() { return transform_.get_scale(); }

        void set_rotation(const float degrees, const glm::vec3 &new_axis) {
            transform_.set_rotation(degrees, new_axis);
        }

        float get_rotation_angle() const { return transform_.get_rotation_angle(); }

        const glm::mat4 &get_local_matrix() const { return transform_.get_local_matrix(); }
        const glm::mat4 &get_world_matrix() const { return transform_.get_world_matrix(); }

        void set_rotation_matrix(const glm::mat4 &rotation_matrix);

        void set_translation_matrix(const glm::mat4 &translation_matrix);

        void set_scale_matrix(const glm::mat4 &scale_matrix);

        virtual void add(Object3D *child) {
            // Check if the child is already in the children list
            auto iterator = std::find(children_.begin(), children_.end(), child);
            if (iterator == children_.end()) {
                if (child->parent_ != nullptr) {
                    child->parent_->remove(child);
                }

                children_.push_back(child);
                child->parent_ = this;
            }
        }

        void remove(Object3D *child) {
            const auto it = std::find(children_.begin(), children_.end(), child);
            if (it != children_.end()) {
                // Remove from children list
                children_.erase(it);
                child->parent_ = nullptr;
            }
        }

        const std::vector<Object3D *> &get_children() const {
            return children_;
        }

        Object3D *get_parent() const {
            return parent_;
        }

        virtual void update(float delta_time) {
        }

        virtual void draw_self(const glm::mat4 &view, const glm::mat4 &projection, uint32_t shader_program,
                               void *renderer_context = nullptr) {
        }

        virtual void draw(const glm::mat4 &view, const glm::mat4 &projection, uint32_t shader_program,
                          void *renderer_context = nullptr) {
            draw_self(view, projection, shader_program, renderer_context);
            for (auto *child: children_) {
                child->draw(view, projection, shader_program, renderer_context);
            }
        }

        void update_world_matrix() {
            const glm::mat4 *parent_world_matrix = nullptr;
            if (parent_ != nullptr) {
                parent_world_matrix = &parent_->get_world_matrix();
            }

            transform_.update_world_matrix(parent_world_matrix);

            // Update Children
            for (auto *child: children_) {
                child->update_world_matrix();
            }
        }

        // overload for the new operator to set the proper memory alignment for glm methods
        void *operator new(size_t size) {
            void *ptr = _aligned_malloc(size, 16);
            if (ptr == nullptr) throw std::bad_alloc();
            return ptr;
        }

        void operator delete(void *ptr) {
            _aligned_free(ptr);
        }

        // Access to transform for advanced operations
        const Transform3D& get_transform() const { return transform_; }

    private:
        std::vector<Object3D *> children_;
        Object3D *parent_ = nullptr;
        std::string name_;

        // The transform component handles all transformation logic
        Transform3D transform_;

        // Mesh is optional
        std::unique_ptr<Mesh> mesh_;
    };
}
