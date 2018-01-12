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
private:
	GLuint framebuffer, averageFBO;
	GLuint texColorBuffer, averageColorBuffer;
	GLuint rbo, averageRBO;
	GLsizei width, height;
	Shader *screenShader, *averageShader;
	void updateTexture(const GLsizei width, const GLsizei height);
	void updateRenderObject(const GLsizei width, const GLsizei height) const;
public:
	Postprocess();
	~Postprocess();
	bool init(GLsizei width, GLsizei height, Shader* screen, Shader* average);
	void update(const GLsizei width, const GLsizei height);
	void render(float exposure) const;
	float findAverage(float prevExposure);
	GLuint getFramebuffer() const;
	GLuint getTexColorBuffer() const;
	GLuint getRbo() const;
};

