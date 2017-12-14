#pragma once
#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"
#include <assimp/types.h>

using namespace std;

class Texture
{
public:
	GLuint id;
	string type;
	string path;
	Texture();
	bool loadTexture(const char* path, string directory);
	void white();
	static void activate(GLuint id);
	~Texture();
};

