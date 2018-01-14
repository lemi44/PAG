#pragma once
#include <glad\glad.h>
#include "Shader.h"
#include "Skybox.h"

class Deferred
{
	GLuint gBuffer; //FBO
	GLuint gBufferRBO;
	GLuint gPosition, gNormal, gAlbedoSpec, gReflection;
public:
	Deferred();
	bool init(GLsizei width, GLsizei height);
	void update(GLsizei width, GLsizei height) const;
	void updateTextures(GLsizei width, GLsizei height) const;
	void updateRBO(GLsizei width, GLsizei height) const;
	GLuint getGBuffer() const;
	void bindTextures(Shader* shader, Skybox* skybox) const;
	Shader *shader;
	~Deferred();
};

