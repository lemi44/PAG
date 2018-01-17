#pragma once
#include "BaseLight.h"
class DirectionalLight :
	public BaseLight
{
public:
	DirectionalLight();
	DirectionalLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 direction);
	~DirectionalLight();
	void draw(Shader* shader, const ViewProjection wvp, const Transform model, const bool gui) final;
	void drawColor(Shader * shader, const ViewProjection wvp) final;
	void setupShader(Shader* shader) final;
	light_type getType() const final;
	glm::vec3 local_direction;
	glm::vec3 real_direction;
};

