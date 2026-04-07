#pragma once
#include <vector>

class Entity 
{
	// vec3 position private
	// model matrix
	// view matrix
	// projection matrix
public:
	// Identity matrix
	std::vector<Entity> children;

	void remove_child(Entity* entity);

	void set_position(float x, float y, float z);
	void set_position(/* vec3 */);

	void set_scale(float scalar);
	void set_scale(float scalar_x, float scalar_y, float scalar_z);

	// Rotation on all axis XYZ
	void set_rotation(float radians);

	// Rotation on only the X and/or Y and/or Z
	void set_rotation(float x_radians, float y_radians, float z_radians);
	
};
