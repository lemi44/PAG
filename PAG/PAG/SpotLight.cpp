#include "SpotLight.h"



SpotLight::SpotLight() : BaseLight()
{
	position = glm::vec3(0.0f, 1.0f, 2.0f);
	direction = glm::vec3(0.0f, 0.0f, 1.0f);
	constant = 1.0f;
	linear = 0.09f;
	quadratic = 0.032f;
	cutOff = glm::cos(glm::radians(12.5f));
	outerCutOff = glm::cos(glm::radians(15.0f));
}

SpotLight::SpotLight(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular,
	const glm::vec3 position, const glm::vec3 direction, const float cutOff,
	const float outerCutOff, const float constant, const float linear, const float quadratic) :
	BaseLight(ambient, diffuse, specular), position(position), direction(direction),
	cutOff(cutOff), outerCutOff(outerCutOff), constant(constant), linear(linear), quadratic(quadratic)
{
}


SpotLight::~SpotLight()
{
}

void SpotLight::setupShader(Shader* shader)
{
	shader->setVec3("spotLight.direction", direction);
	shader->setVec3("spotLight.direction", direction);
	shader->setFloat("spotLight.cutOff", cutOff);
	shader->setFloat("spotLight.outerCutOff", outerCutOff);
	shader->setFloat("spotLight.constant", constant);
	shader->setFloat("spotLight.linear", linear);
	shader->setFloat("spotLight.quadratic", quadratic);
	shader->setVec3("spotLight.ambient", ambient);
	shader->setVec3("spotLight.diffuse", diffuse);
	shader->setVec3("spotLight.specular", specular);
}
