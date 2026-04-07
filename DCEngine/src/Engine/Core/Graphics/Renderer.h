#pragma once
#include <cstdint>
#include <GL/glew.h>
#include "../Object3D.h"
#include "../../Helpers/Camera.h"



class Renderer
{
	uint32_t program_id_;
	int32_t uniform_mvp_;
public:
	Renderer(uint32_t program_id_);

	void Render(Object3D& scene, Camera& camera);

};

