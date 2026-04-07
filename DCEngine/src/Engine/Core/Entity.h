#pragma once
#include <vector>

class Entity 
{
public:
	// Identity matrix
	std::vector<Entity> children;

	void remove_child(Entity* entity);
	
};
