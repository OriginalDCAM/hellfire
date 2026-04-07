#include "DCraft/Addons/PerspectiveCamera.h"

namespace DCraft
{

    glm::mat4 PerspectiveCamera::get_projection_matrix()
    {
        return projection_matrix_;
    }

    void PerspectiveCamera::update(float delta_time)
    {
        Camera::update(delta_time);
        update_projection_matrix();
    }

    void PerspectiveCamera::set_fov(float fov)
    {
        fov_ = fov;
        update_projection_matrix();
    }

    void PerspectiveCamera::set_aspect_ratio(float aspect)
    {
        aspect_ = aspect;
        update_projection_matrix();
    }

    void PerspectiveCamera::set_clip_planes(float near_plane, float far_plane)
    {
        near_ = near_plane;
        far_ = far_plane;
        update_projection_matrix();
    }

    void PerspectiveCamera::process_mouse_scroll(float y_offset)
    {
        set_fov(get_zoom());
        Camera::process_mouse_scroll(y_offset);
    }

    float PerspectiveCamera::get_aspect_ratio() const {
        return aspect_;
    }

    void PerspectiveCamera::update_view_matrix() {
        
    }

    void PerspectiveCamera::set_target(float x, float y, float z) {
        target_ = glm::vec3(x, y, z);

        glm::vec3 direction = glm::normalize(target_ - get_position());

        pitch_ = glm::degrees(asin(direction.y));
        yaw_ = glm::degrees(atan2(direction.z, direction.x));
        
        update_camera_vectors();
    }

    glm::vec3 PerspectiveCamera::get_target() {
        return target_;
    }
}
