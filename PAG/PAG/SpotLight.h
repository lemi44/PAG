#pragma once
#include "BaseLight.h"
class SpotLight :
	public BaseLight
{
public:
	SpotLight();
	SpotLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position,
		glm::vec3 direction, float cutOff, float outerCutOff, float constant, float linear,
		float quadratic);
	~SpotLight();
	void setupShader(Shader* shader) final;
	glm::vec3 position;
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
};

