#include "DirectionalLight.h"
#include <glm/glm.hpp>


DirectionalLight::DirectionalLight() : BaseLight()
{
	real_direction = local_direction = glm::vec3(0.f, -1.f, 0.f);
}

DirectionalLight::DirectionalLight(
	const glm::vec3 ambient,
	const glm::vec3 diffuse,
	const glm::vec3 specular,
	const glm::vec3 direction) :
	BaseLight(ambient, diffuse, specular), local_direction(direction), real_direction(direction)
{
}


DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::draw(Shader* shader, const ViewProjection wvp, const Transform model, const bool gui)
{
	const auto tmp_dir = wvp.view * model.getMatrix() * glm::vec4(local_direction, 0.0);
	real_direction = glm::normalize(glm::vec3(tmp_dir));
	if (gui)
		drawColor(shader, wvp);
	else
		setupShader(shader);
}

void DirectionalLight::drawColor(Shader * shader, const ViewProjection wvp)
{
	shader->setMat4("model", glm::mat4(1.0f));
	shader->setMat4("wvp", wvp.view);
	shader->setMat4("projection", wvp.projection);
	meshes_[0].drawColor(shader, id);
}

void DirectionalLight::setupShader(Shader* shader)
{
	shader->setVec3("dirLight.direction", real_direction);
	shader->setVec3("dirLight.ambient", ambient);
	shader->setVec3("dirLight.diffuse", diffuse);
	shader->setVec3("dirLight.specular", specular);
}

light_type DirectionalLight::getType() const
{
	return directional;
}


