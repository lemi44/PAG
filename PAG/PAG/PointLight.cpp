#include "PointLight.h"
#include "Logger.h"


PointLight::PointLight() : BaseLight()
{
	real_position = local_position = glm::vec3(0.f);
	constant = 1.0f;
	linear = 0.09f;
	quadratic = 0.032f;
}

PointLight::PointLight(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular,
	const glm::vec3 position, const float constant, const float linear, const float quadratic) :
	BaseLight(ambient, diffuse, specular), local_position(position), real_position(position), constant(constant), linear(linear),
	quadratic(quadratic)
{
}

void PointLight::draw(Shader* shader, const ViewProjection wvp, const Transform model, const bool gui)
{
	const auto tmp_dir = model.getMatrix() * glm::vec4(local_position, 1.0);
	real_position = glm::vec3(tmp_dir);
	if (gui)
		drawColor(shader, wvp);
	else
		setupShader(shader);
}

void PointLight::drawColor(Shader * shader, const ViewProjection wvp)
{
	shader->setMat4("model", Transform::origin().translate(real_position).getMatrix());
	shader->setMat4("view", wvp.view);
	shader->setMat4("projection", wvp.projection);
	meshes_[0].drawColor(shader, id);
}

light_type PointLight::getType() const
{
	return point;
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
	shader->setVec3(string_format("pointLights[%d].position", index), real_position);
	shader->setFloat(string_format("pointLights[%d].constant", index), constant);
	shader->setFloat(string_format("pointLights[%d].linear", index), linear);
	shader->setFloat(string_format("pointLights[%d].quadratic", index), quadratic);
	shader->setVec3(string_format("pointLights[%d].ambient", index), ambient);
	shader->setVec3(string_format("pointLights[%d].diffuse", index), diffuse);
	shader->setVec3(string_format("pointLights[%d].specular", index), specular);
}
