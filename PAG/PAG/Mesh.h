#pragma once
#include <vector>
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_vec2.hpp>
#include "Shader.h"
#include "Texture.h"
#include "Material.h"

using namespace std;

struct Vertex
{
	Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex) : position(pos), normal(norm), tex_coords(tex){}
	Vertex(glm::vec3 pos) : position(pos){}
	Vertex(){}
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coords;
};

class Mesh
{
private:
	/*  Render data  */
	// ReSharper disable CppInconsistentNaming
	unsigned int VAO, VBO, EBO;
	// ReSharper restore CppInconsistentNaming
	/*  Functions    */
	void setupMesh();
public:
	/*  Mesh Data  */
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	Material material;
	bool outline;
	/*  Functions  */
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Material material);
	void draw(Shader* shader);
	void drawColor(Shader* shader, GLuint id) const;

	virtual ~Mesh();
};

