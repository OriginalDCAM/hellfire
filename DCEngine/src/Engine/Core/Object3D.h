#pragma once
#include <glm/detail/type_vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

#include "Graphics/Model.h"

namespace DCraft
{

    class Object3D
    {
    public:
        virtual ~Object3D()
        {
            for (auto* child : children_)
            {
                delete child;
            }
            children_.clear();

        }
        Object3D() : position_(0.0f, 0.0f, 0.0f), scale_(1.0, 1.0f, 1.0f), rotation_axis_(0.0f, 0.0f, 0.0f), rotation_angle_(0.0f), world_matrix_(1.0f)
        {
            model_.transform = glm::mat4(1.0f);
        }

        void set_name(std::string name)
        {
            name_ = std::move(name);
        }

        std::string& get_name()
        {
            return name_;
        }


        const glm::vec3& get_position() const { return position_; }

        void set_position(const glm::vec3& new_position)
        {
            position_ = new_position;
            update_model_matrix();
        }

        void set_position(const float x, const float y, const float z)
        {
            position_ = glm::vec3(x, y, z);
            update_model_matrix();
        }


        void set_scale(const glm::vec3 new_scale)
        {
            scale_ = new_scale;
            update_model_matrix();
        }

        glm::vec3 get_current_scale() { return scale_; }

        void set_rotation(const float degrees, glm::vec3 new_axis)
        {
            rotation_angle_ = degrees * glm::pi<float>() / 180;
            rotation_axis_ = new_axis;
            update_model_matrix();
        }

        float get_rotation_angle() const { return rotation_angle_; }

        //VA* get_vao() const { return vao_; }

        const std::vector<unsigned int>& get_indices() const
        {
            if (model_.meshes.empty()) {
                static const std::vector<unsigned int> empty_indices;
                return empty_indices;
            }

            return model_.meshes[0].indices;
        }

        const glm::mat4& get_model_matrix() const { return model_.transform; }

        Model& get_model() { return model_; }

        void add(Object3D* child)
        {
            // Check if the child is already in the children list
            auto iterator = std::find(children_.begin(), children_.end(), child);
            if (iterator == children_.end())
            {
                if (child->parent_ != nullptr)
                {
                    child->parent_->remove(child);
                }

                children_.push_back(child);

                child->parent_ = this;
            }
        }

        void remove(Object3D* child)
        {
            auto iterator = std::find(children_.begin(), children_.end(), child);
            if (iterator != children_.end())
            {
                // Remove from children list
                children_.erase(iterator);

                child->parent_ = nullptr;
            }
        }

        const std::vector<Object3D*>& get_children() const
        {
            return children_;
        }

        Object3D* get_parent() const
        {
            return parent_;
        }

        virtual void update(float delta_time) {}

        virtual void draw_self(const glm::mat4& view, const glm::mat4& projection, uint32_t shader_program) {}


        virtual void draw(const glm::mat4& view, const glm::mat4& projection, uint32_t shader_program)
        {
            draw_self(view, projection, shader_program);
            for (auto* child : children_)
            {
                child->draw(view, projection, shader_program);
            }
        }

        void update_world_matrix()
        {
            glm::mat4 world_matrix = model_.transform;

            if (parent_ != nullptr)
            {
                world_matrix = parent_->get_world_matrix() * world_matrix;
            }

            world_matrix_ = world_matrix;

            // Update Children
            for (auto* child : children_)
            {
                child->update_world_matrix();
            }

        }

        const glm::mat4& get_world_matrix() const {
            return world_matrix_;
        }

        void update_model_matrix()
        {
            model_.transform = glm::mat4(1.0f);
            model_.transform = glm::translate(model_.transform, position_);

            if (glm::length(rotation_axis_) > 0.0001f) {
                glm::vec3 normalized_axis = glm::normalize(rotation_axis_);
                model_.transform = glm::rotate(model_.transform, rotation_angle_, normalized_axis);
            }

            model_.transform = glm::scale(model_.transform, scale_);
        }

    private:
        std::vector<Object3D*> children_;
        Object3D* parent_ = nullptr;
        std::string name_;

        glm::vec3 position_;
        glm::vec3 scale_;
        glm::vec3 rotation_axis_;
        float rotation_angle_;
        //VA* vao_;
        Model model_;


    protected:
        glm::mat4 world_matrix_ = glm::mat4(1.0f);
    };
}
