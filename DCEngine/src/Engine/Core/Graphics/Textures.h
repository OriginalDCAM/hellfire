#pragma once
#include <cstdint>
#include <string>

struct Texture 
{
	uint32_t id;
	std::string type;

	Texture(const std::string& filename);


private:
	bool load_from_file(const std::string& filename);
};

inline Texture::Texture(const std::string& filename)
{
	if (!load_from_file(filename)) throw std::exception("TEXTURE: Cannot load texture from file");
}
