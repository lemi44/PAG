#pragma once
#include <string>
#include <vector>
#include <glm/mat4x4.hpp>
#include "Shader.h"

class Skybox
{
	unsigned int cubemapTexture;
	unsigned int skyboxVAO;
	unsigned int skyboxVBO;
	Shader* shader;
public:
	Skybox();
	void Skybox::loadCubemap(std::vector<std::string> faces);
	void drawSkybox(glm::mat4 const wvp);
	~Skybox();
	void init(Shader* shader);
	GLuint getSkybox() const;
};

