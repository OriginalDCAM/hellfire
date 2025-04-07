#pragma once
#include <cstdint>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>

class VB
{
	uint32_t m_renderer_id_;
public:
	VB();
	~VB();
	void bind();
	void unbind();

	template<typename T>
	void pass_data(const T* data, size_t count) const
	{
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), data, GL_STATIC_DRAW);
	}

	template<typename T>
	void pass_data(const std::vector<T>& data) const
	{
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
	}
};
