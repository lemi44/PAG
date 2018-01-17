#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>
#include "Shader.h"
#include "Deferred.h"

class Deferred;

class SSAO
{
	GLuint noiseTexture;
	GLuint ssaoFBO;
	GLuint ssaoColorBuffer;
	GLuint ssaoBlurFBO, ssaoColorBufferBlur;
	Shader* shaderSSAO, *shaderSSAOBlur;
	std::vector<glm::vec3> ssaoKernel;
public:
	SSAO();
	bool init(GLsizei width, GLsizei height, Shader* ssaoShader, Shader* ssaoShaderBlur);
	void render(Deferred* deferred, glm::mat4 projection);
	GLuint getSSAO() const;
	~SSAO();
};
