#pragma once
#include "BaseLight.h"
class PointLight :
	public BaseLight
{
public:
	PointLight();
	PointLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position, float constant, float linear, float quadratic);
	~PointLight();
	void setupShader(Shader* shader) final;
	void setupShader(Shader* shader, int index) const;
	glm::vec3 position;
	float constant;
	float linear;
	float quadratic;
};

