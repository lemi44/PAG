#pragma once
#include <vector>
#include "Texture.h"

struct Material
{
	std::vector<Texture> textures;
	float shininess;
};

