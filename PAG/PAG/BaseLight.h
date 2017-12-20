#pragma once
#include <glm/detail/type_vec3.hpp>
#include "Shader.h"
class BaseLight
{
public:
	BaseLight();
	BaseLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	virtual ~BaseLight();
	virtual void setupShader(Shader* shader) = 0;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

