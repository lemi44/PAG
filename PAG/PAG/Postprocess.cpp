#include "Postprocess.h"
#include "Logger.h"
#include <vector>


float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

Postprocess::Postprocess()
{
}


Postprocess::~Postprocess()
{
	glDeleteFramebuffers(1, &framebuffer);
}

void Postprocess::updateTexture(const GLsizei width, const GLsizei height)
{
	this->width = width;
	this->height = height;
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Postprocess::updateRenderObject(const GLsizei width, const GLsizei height) const
{
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

bool Postprocess::init(const GLsizei width, const GLsizei height, Shader* screen, Shader* average)
{
	auto success = true;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// generate texture
	glGenTextures(1, &texColorBuffer);
	updateTexture(width, height);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	glGenRenderbuffers(1, &rbo);
	updateRenderObject(width, height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Logger::logError("Framebuffer is not complete!");
		success = false;
	}

	glGenFramebuffers(1, &averageFBO);
	// generate texture
	glGenTextures(1, &averageColorBuffer);
	glGenRenderbuffers(1, &averageRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, averageFBO);
	// Reset color buffer dimensions
	glBindTexture(GL_TEXTURE_2D, averageColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, averageColorBuffer, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, averageRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, averageRBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Logger::logError("Framebuffer is not complete!");
		success = false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	screenShader = screen;
	averageShader = average;

	return success;
}

void Postprocess::update(const GLsizei width, const GLsizei height)
{
	updateTexture(width, height);
	updateRenderObject(width, height);
}

void Postprocess::render(float exposure) const
{
	glBindTexture(GL_TEXTURE_2D, averageColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);

	glBindFramebuffer(GL_FRAMEBUFFER, averageFBO);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	screenShader->use();
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	screenShader->setFloat("exposure", exposure);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

float Postprocess::findAverage(float prevExposure)
{
	GLuint texWidth = width, texHeight = height;
	GLboolean change = true;
	averageShader->use();
	GLfloat luminescence[3];

	glBindTexture(GL_TEXTURE_2D, averageColorBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);

	int max_level;

	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, &max_level);
	int max_mipmap = -1;
	for (int i = 0; i < max_level; ++i)
	{
		int width;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, i, GL_TEXTURE_WIDTH, &width);
		if (0 == width)
		{
			max_mipmap = i - 1;
			break;
		}
	}
	// Once done read the luminescence value of 1x1 texture
	glGetTexImage(GL_TEXTURE_2D, max_mipmap, GL_RGB, GL_FLOAT, &luminescence[0]);

	GLfloat lum = 0.2126 * luminescence[0] + 0.7152 * luminescence[1] + 0.0722 * luminescence[2];
	auto ret = lerp(prevExposure, 0.5 / lum, 0.05); // slowly adjust exposure based on average brightness

	return ret;
}

GLuint Postprocess::getFramebuffer() const
{
	return framebuffer;
}

GLuint Postprocess::getTexColorBuffer() const
{
	return texColorBuffer;
}

GLuint Postprocess::getRbo() const
{
	return rbo;
}
