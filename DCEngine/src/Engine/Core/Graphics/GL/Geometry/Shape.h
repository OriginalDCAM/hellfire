#pragma once
#include "../../../Object3D.h"

class Shape : public Object3D
{
public:
	virtual void set_vertices();
	virtual void set_colors();
	virtual void set_uvs();
protected:
	std::vector<float >vertices_;
	std::vector<float> colors_;
	std::vector<unsigned int> indices_;
};

