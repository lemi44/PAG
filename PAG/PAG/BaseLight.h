#pragma once
#include <glm/detail/type_vec3.hpp>
#include "Shader.h"
#include "Drawable.h"
#include "Mesh.h"
enum light_type
{
	directional,
	point,
	spot
};
class BaseLight : public Drawable
{
protected:
	std::vector<Vertex> helperMeshVertices_ =
	{
		Vertex(glm::vec3(-1.0, -1.0, 1.0)),
		Vertex(glm::vec3(1.0, -1.0, 1.0)),
		Vertex(glm::vec3(1.0, 1.0, 1.0)),
		Vertex(glm::vec3(-1.0, 1.0, 1.0)),
		Vertex(glm::vec3(-1.0, -1.0, -1.0)),
		Vertex(glm::vec3(1.0, -1.0, -1.0)),
		Vertex(glm::vec3(1.0, 1.0, -1.0)),
		Vertex(glm::vec3(-1.0, 1.0, -1.0))
	};
	std::vector<unsigned int> helperMeshIndices_ = { 0, 1, 2, 0, 2, 3, //front
		4, 7, 6, 4, 6, 5, //back
		1, 5, 6, 1, 6, 2, //right
		4, 0, 3, 4, 3, 7, //left
		3, 2, 6, 3, 6, 7, //upper
		1, 0, 4, 1, 4, 5 }; //bottom;
	std::vector<Mesh> meshes_{ Mesh(helperMeshVertices_, helperMeshIndices_, Material()) };
public:
	BaseLight();
	BaseLight(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	virtual ~BaseLight();
	virtual void setupShader(Shader* shader) = 0;
	bool isLight() const override;
	virtual light_type getType() const = 0;
	std::vector<Mesh>& getMeshes() override;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

