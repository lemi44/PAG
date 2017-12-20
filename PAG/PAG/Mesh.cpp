#include "Mesh.h"
#include "Logger.h"


void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(0));
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tex_coords)));

	glBindVertexArray(0);
}

Mesh::Mesh(const vector<Vertex> vertices, const vector<unsigned int> indices, const Material material) : outline(false)
{
	this->vertices = vertices;
	this->indices = indices;
	this->material = material;

	setupMesh();
}

void Mesh::draw(Shader* shader)
{
	auto diffuse_n = 0;
	auto specular_n = 0;
	for (unsigned int i = 0; i < material.textures.size(); i++)
	{
		auto number = -1;
		const auto name = material.textures[i].type;
		if (name == "texture_diffuse")
			number = diffuse_n++;
		else if (name == "texture_specular")
			number = specular_n++;

		if (number != 1)
		{
			Logger::logWarning(string_format("Ignored texture %s of type %s", material.textures[i].path.c_str(), name.c_str()));
		}
		else
		{
			glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
												 // retrieve texture number (the N in diffuse_textureN)
			shader->setInt("material." + name.substr(8), i);
			glBindTexture(GL_TEXTURE_2D, material.textures[0].id);
		}
	}
	if (diffuse_n < 1) Logger::logWarning("No diffuse texture loaded!");
	if (specular_n < 1) Logger::logWarning("No specular texture loaded!");
	glActiveTexture(GL_TEXTURE0);

	if (outline)
	{
		glColorMask(GL_TRUE, GL_TRUE, GL_FALSE, GL_TRUE);
	}
	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	if (outline)
	{
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
}

void Mesh::drawColor(Shader* shader, const GLuint id) const
{
	// Convert "i", the integer mesh ID, into an RGB color
	const int r = (id & 0x000000FF) >> 0;
	const int g = (id & 0x0000FF00) >> 8;
	const int b = (id & 0x00FF0000) >> 16;
	shader->setVec4("PickingColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


Mesh::~Mesh()
{
}
