#pragma once
#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"
#include <assimp/types.h>
#include <assimp/texture.h>

using namespace std;

class Texture
{
public:
	GLuint id;
	string type;
	string path;
	Texture();
	bool loadTexture(const char* path, string directory);
	bool loadTexture(aiTexture *texture);
	void white();
	static void activate(GLuint id);
	~Texture();
};

