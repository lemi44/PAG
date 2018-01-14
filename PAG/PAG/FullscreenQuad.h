#pragma once
#include <glad/glad.h>

class FullscreenQuad
{
	static GLuint quadVAO, quadVBO;
public:
	static void renderQuad();
};

