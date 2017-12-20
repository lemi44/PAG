#include "BaseLight.h"



BaseLight::BaseLight()
{
	ambient = glm::vec3(0.2f);
	diffuse = glm::vec3(0.8f);
	specular = glm::vec3(0.0f);
}

BaseLight::BaseLight(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular) :
	ambient(ambient), diffuse(diffuse), specular(specular)
{
}


BaseLight::~BaseLight()
{
}
