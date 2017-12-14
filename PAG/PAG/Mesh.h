#pragma once
#include <vector>
#include <glm/detail/type_vec3.hpp>
#include <glm/detail/type_vec2.hpp>
#include "Shader.h"
#include "Texture.h"

using namespace std;

struct Vertex
{
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
	vector<Texture> textures;
	bool outline;
	/*  Functions  */
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	void draw(Shader* shader);
	void drawColor(Shader* shader, GLuint id) const;

	virtual ~Mesh();
};

