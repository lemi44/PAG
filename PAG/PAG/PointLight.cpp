#include "PointLight.h"
#include "Logger.h"


PointLight::PointLight() : BaseLight()
{
	position = glm::vec3(0.f);
	constant = 1.0f;
	linear = 0.09f;
	quadratic = 0.032f;
}

PointLight::PointLight(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular,
	const glm::vec3 position, const float constant, const float linear, const float quadratic) :
	BaseLight(ambient, diffuse, specular), position(position), constant(constant), linear(linear),
	quadratic(quadratic)
{
}


PointLight::~PointLight()
{
}

void PointLight::setupShader(Shader* shader)
{
	setupShader(shader, 0);
}

void PointLight::setupShader(Shader* shader, const int index) const
{
	shader->setVec3(string_format("pointLights[%d].position", index), position);
	shader->setFloat(string_format("pointLights[%d].constant", index), constant);
	shader->setFloat(string_format("pointLights[%d].linear", index), linear);
	shader->setFloat(string_format("pointLights[%d].quadratic", index), quadratic);
	shader->setVec3(string_format("pointLights[%d].ambient", index), ambient);
	shader->setVec3(string_format("pointLights[%d].diffuse", index), diffuse);
	shader->setVec3(string_format("pointLights[%d].specular", index), specular);
}
