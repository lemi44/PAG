#include "SpotLight.h"



SpotLight::SpotLight() : BaseLight()
{
	local_position = real_position = glm::vec3(0.0f, 1.0f, 2.0f);
	local_direction = real_direction = glm::vec3(0.0f, 0.0f, 1.0f);
	constant = 1.0f;
	linear = 0.09f;
	quadratic = 0.032f;
	cutOff = glm::cos(glm::radians(12.5f));
	outerCutOff = glm::cos(glm::radians(15.0f));
}

SpotLight::SpotLight(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular,
	const glm::vec3 position, const glm::vec3 direction, const float cutOff,
	const float outerCutOff, const float constant, const float linear, const float quadratic) :
	BaseLight(ambient, diffuse, specular), local_position(position), real_position(position),
	local_direction(direction), real_direction(direction), cutOff(cutOff),
	outerCutOff(outerCutOff), constant(constant), linear(linear), quadratic(quadratic)
{
}

void SpotLight::draw(Shader* shader, const ViewProjection wvp, const Transform model, const bool gui)
{
	const auto tmp_pos = wvp.view * model.getMatrix() * glm::vec4(local_position, 1.0f);
	real_position = glm::vec3(tmp_pos);
	const auto tmp_dir = wvp.view * model.getMatrix() * glm::vec4(local_direction, 0.0f);
	real_direction = glm::normalize(glm::vec3(tmp_dir));
	if (gui)
		drawColor(shader, wvp);
	else
		setupShader(shader);
}

void SpotLight::drawColor(Shader * shader, const ViewProjection wvp)
{
	const auto world_pos = glm::inverse(wvp.view) * glm::vec4(real_position, 1.0f);
	shader->setMat4("model", Transform::origin().translate(world_pos).getMatrix());
	shader->setMat4("view", wvp.view);
	shader->setMat4("projection", wvp.projection);
	meshes_[0].drawColor(shader, id);
}

light_type SpotLight::getType() const
{
	return spot;
}


SpotLight::~SpotLight()
{
}

void SpotLight::setupShader(Shader* shader)
{
	shader->setVec3("spotLight.position", real_position);
	shader->setVec3("spotLight.direction", real_direction);
	shader->setFloat("spotLight.cutOff", cutOff);
	shader->setFloat("spotLight.outerCutOff", outerCutOff);
	shader->setFloat("spotLight.constant", constant);
	shader->setFloat("spotLight.linear", linear);
	shader->setFloat("spotLight.quadratic", quadratic);
	shader->setVec3("spotLight.ambient", ambient);
	shader->setVec3("spotLight.diffuse", diffuse);
	shader->setVec3("spotLight.specular", specular);
}
