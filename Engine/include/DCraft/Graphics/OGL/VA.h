#pragma once
#include <cstdint>
/*
 *	Vertex Array Class
 */
class VA
{
	uint32_t m_renderer_id_;
public:
	VA();
	~VA();
	void bind() const;
	void unbind();

	uint32_t get_id() const;
	
};

