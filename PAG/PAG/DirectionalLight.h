#pragma once
#include "BaseLight.h"
class DirectionalLight :
	public BaseLight
{
public:
	DirectionalLight();
	DirectionalLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction);
	~DirectionalLight();
	void setupShader(Shader* shader) final;
	glm::vec3 direction;
};

