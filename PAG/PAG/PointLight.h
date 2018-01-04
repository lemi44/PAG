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
	void draw(Shader* shader, const Transform wvp, const Transform model, const bool gui) final;
	void drawColor(Shader * shader, const Transform wvp) final;
	light_type getType() const final;
	glm::vec3 local_position;
	glm::vec3 real_position;
	float constant;
	float linear;
	float quadratic;
};

