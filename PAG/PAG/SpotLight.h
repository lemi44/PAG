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
	void draw(Shader* shader, const ViewProjection wvp, const Transform model, const bool gui) final;
	void drawColor(Shader * shader, const ViewProjection wvp) final;
	light_type getType() const final;
	glm::vec3 real_position;
	glm::vec3 local_position;
	glm::vec3 local_direction;
	glm::vec3 real_direction;
	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
};

