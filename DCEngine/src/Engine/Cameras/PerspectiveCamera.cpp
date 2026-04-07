#include "PerspectiveCamera.h"

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
}
