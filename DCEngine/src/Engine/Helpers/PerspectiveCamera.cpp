#include "PerspectiveCamera.h"

glm::mat4 PerspectiveCamera::get_projection_matrix()
{
	return projection_matrix_;
}

void PerspectiveCamera::update()
{
        Camera::update();
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

void PerspectiveCamera::set_clip_planes(float near, float far)
{
	near_ = near;
	far_ = far;
	update_projection_matrix();
}

void PerspectiveCamera::process_mouse_scroll(float y_offset)
{
	set_fov(get_zoom());
	Camera::process_mouse_scroll(y_offset);
}
