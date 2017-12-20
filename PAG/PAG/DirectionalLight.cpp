#include "DirectionalLight.h"



DirectionalLight::DirectionalLight() : BaseLight()
{
	direction = glm::vec3(0.f, -1.f, 0.f);
}

DirectionalLight::DirectionalLight(
	const glm::vec3 ambient,
	const glm::vec3 diffuse,
	const glm::vec3 specular,
	const glm::vec3 direction) :
	BaseLight(ambient, diffuse, specular), direction(direction)
{
}


DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::setupShader(Shader* shader)
{
	shader->setVec3("dirLight.direction", direction);
	shader->setVec3("dirLight.ambient", ambient);
	shader->setVec3("dirLight.diffuse", diffuse);
	shader->setVec3("dirLight.specular", specular);
}


