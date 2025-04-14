#pragma once

#include <DCraft/Structs/Object3D.h>
#include <functional>
#include <vector>
#include <string>

#include "DCraft/Graphics/Lights/Light.h"

namespace DCraft {
    class AnimationSystem {
    public:
        // Animation callback type
        using AnimationCallback = std::function<void(Object3D *, float, float)>;

        // Animation data structure
        struct Animation {
            std::string name; // Name for identification
            Object3D *target; // Target object
            float duration; // Duration in seconds
            float elapsed_time = 0.0f; // Current elapsed time
            bool loop; // Whether to loop
            bool active = true; // Whether animation is active
            AnimationCallback callback; // The animation function
        };

        // Add an animation
        void add_animation(const std::string &name, Object3D *target, float duration,
                           AnimationCallback callback, bool loop = false) {
            animations_.push_back({name, target, duration, 0.0f, loop, true, callback});
        }

        // Update all animations
        void update(float dt) {
            for (auto it = animations_.begin(); it != animations_.end();) {
                auto &anim = *it;

                if (!anim.active) {
                    it = animations_.erase(it);
                    continue;
                }

                anim.elapsed_time += dt;

                if (anim.elapsed_time >= anim.duration) {
                    if (anim.loop) {
                        // Loop back
                        anim.elapsed_time = fmodf(anim.elapsed_time, anim.duration);
                    } else {
                        // Ensure we get the final frame
                        anim.callback(anim.target, 1.0f, dt);
                        it = animations_.erase(it);
                        continue;
                    }
                }

                float progress = anim.elapsed_time / anim.duration;
                anim.callback(anim.target, progress, dt);

                ++it;
            }
        }

        // Pause/resume an animation by name
        void set_animation_active(const std::string &name, bool active) {
            for (auto &anim: animations_) {
                if (anim.name == name) {
                    anim.active = active;
                }
            }
        }

        // Stop and remove an animation by name
        void remove_animation(const std::string &name) {
            animations_.erase(
                std::remove_if(animations_.begin(), animations_.end(),
                               [&name](const Animation &anim) { return anim.name == name; }),
                animations_.end());
        }

        // Stop all animations for a specific object
        void remove_animations_for_object(Object3D *target) {
            animations_.erase(
                std::remove_if(animations_.begin(), animations_.end(),
                               [target](const Animation &anim) { return anim.target == target; }),
                animations_.end());
        }

        // Helper methods for common animations

        void create_rotation(const std::string &name, Object3D *target,
                             float x_speed, float y_speed, float z_speed) {
            add_animation(name, target, 1.0f,
                          [x_speed, y_speed, z_speed](Object3D *obj, float progress, float dt) {
                              glm::vec3 current_rotation = obj->get_rotation();

                              // Apply rotation based on speeds and delta time
                              current_rotation.x += x_speed * dt;
                              current_rotation.y += y_speed * dt;
                              current_rotation.z += z_speed * dt;

                              // Keep rotations in range 0-360
                              current_rotation.x = fmodf(current_rotation.x, 360.0f);
                              current_rotation.y = fmodf(current_rotation.y, 360.0f);
                              current_rotation.z = fmodf(current_rotation.z, 360.0f);

                              obj->set_rotation(current_rotation);
                          }, true);
        }

        // Pulsing scale animation
        void create_pulsing_scale(const std::string &name, Object3D *target,
                                  float duration, float min_scale, float max_scale) {
            add_animation(name, target, duration,
                          [min_scale, max_scale](Object3D *obj, float progress, float dt) {
                              // Sine wave pulsing
                              float scale = min_scale + (max_scale - min_scale) *
                                            (0.5f + 0.5f * sinf(progress * 6.28318f));

                              obj->set_scale(glm::vec3(scale));
                          }, true);
        }

        // Moving in a circular path
        void create_circular_path(const std::string &name, Object3D *target,
                                  float duration, float radius, bool face_direction = true) {
            // Store the original position
            glm::vec3 center = target->get_position();

            add_animation(name, target, duration,
                          [center, radius, face_direction](Object3D *obj, float progress, float dt) {
                              // Calculate angle based on progress
                              float angle = progress * 6.28318f; // 2*PI

                              // Calculate new position
                              glm::vec3 new_pos = center + glm::vec3(
                                                      radius * cosf(angle),
                                                      0.0f,
                                                      radius * sinf(angle)
                                                  );

                              obj->set_position(new_pos);

                              // Make object face movement direction
                              if (face_direction) {
                                  // Direction tangent to circle
                                  glm::vec3 direction = glm::vec3(-sinf(angle), 0.0f, cosf(angle));
                                  glm::vec3 target_pos = new_pos + direction;
                                  obj->look_at(target_pos, glm::vec3(0.0f, 1.0f, 0.0f));
                              }
                          }, true);
        }

        // Oscillating movement (up and down, or side to side)
        void create_oscillating_movement(const std::string &name, Object3D *target,
                                         float duration, const glm::vec3 &axis, float amplitude) {
            // Store the original position
            glm::vec3 start_pos = target->get_position();

            add_animation(name, target, duration,
                          [start_pos, axis, amplitude](Object3D *obj, float progress, float dt) {
                              // Sine wave oscillation
                              float offset = amplitude * sinf(progress * 6.28318f);

                              // Calculate new position
                              glm::vec3 new_pos = start_pos + (axis * offset);
                              obj->set_position(new_pos);
                          }, true);
        }

        void create_intensity_pulse(const std::string &name, Light *target,
                                    float duration, float min_intensity, float max_intensity) {
            add_animation(name, target, duration,
                          [min_intensity, max_intensity](Object3D *obj, float progress, float dt) {
                              // Cast back to Light* since we know this is a light
                              DCraft::Light *light = static_cast<Light *>(obj);

                              // Sine wave pulsing
                              float intensity = min_intensity + (max_intensity - min_intensity) *
                                                (0.5f + 0.5f * sinf(progress * 6.28318f));

                              light->set_intensity(intensity);
                          }, true);
        }

    private:
        std::vector<Animation> animations_;
    };
} // namespace DCraft
