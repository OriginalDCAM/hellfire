#pragma once
#include "Camera.h"

class PerspectiveCamera : public Camera
{
private:
    float fov_;
    float aspect_;
    float near_;
    float far_;
    glm::mat4 projection_matrix_;

public:
    PerspectiveCamera(float fov = ZOOM,
        float aspect = 1.0f,
        float near = 0.1f,
        float far = 100.0f,
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH) :
        Camera(position, up, yaw, pitch),
        fov_(fov),
        aspect_(aspect),
        near_(near),
        far_(far)
    {
        update_projection_matrix();
    }

    glm::mat4 get_projection_matrix() override;
    void update() override;
    // Setters with update
    void set_fov(float fov);
    void set_aspect_ratio(float aspect);
    void set_clip_planes(float near, float far);
    void process_mouse_scroll(float y_offset) override;
private:
    void update_projection_matrix() {
        if (near_ <= 0.0f) {
            std::cout << "ERROR: Near plane must be positive. Setting to 0.1." << std::endl;
            near_ = 0.1f;
        }

        if (far_ <= near_) {
            std::cout << "ERROR: Far plane must be greater than near plane. Setting to near + 100." << std::endl;
            far_ = near_ + 100.0f;
        }

        if (aspect_ <= 0.0f) {
            std::cout << "ERROR: Aspect ratio must be positive. Setting to 1.0." << std::endl;
            aspect_ = 1.0f;
        }

        projection_matrix_ = glm::perspective(glm::radians(fov_), aspect_, near_, far_);
    }
};