#pragma once
#include <glad/glad.h>
#include "Shader.h"

struct QuadVertex
{
	glm::vec2 position;
	glm::vec2 tex_coords;
};

class Postprocess
{
	GLuint framebuffer;
	GLuint texColorBuffer;
	GLuint rbo;
	Shader* screenShader;
	void updateTexture(const GLsizei width, const GLsizei height) const;
	void updateRenderObject(const GLsizei width, const GLsizei height) const;
public:
	Postprocess();
	~Postprocess();
	bool init(GLsizei width, GLsizei height, Shader* shader);
	void update(const GLsizei width, const GLsizei height) const;
	void render(float exposure) const;
	GLuint getFramebuffer() const;
	GLuint getTexColorBuffer() const;
	GLuint getRbo() const;
};

