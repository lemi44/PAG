#include "BaseLight.h"
BaseLight::BaseLight() : Drawable()
{
	ambient = glm::vec3(0.2f);
	diffuse = glm::vec3(0.8f);
	specular = glm::vec3(0.0f);
}

BaseLight::BaseLight(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular) :
	Drawable(), ambient(ambient), diffuse(diffuse), specular(specular)
{
}


BaseLight::~BaseLight()
{
}

bool BaseLight::isLight() const
{
	return true;
}

std::vector<Mesh>& BaseLight::getMeshes()
{
	return meshes_;
}
