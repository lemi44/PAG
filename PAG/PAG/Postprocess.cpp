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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(2, &pingpongFBO[0]);
	// generate texture
	glGenTextures(2, &pingpongColorBuffer[0]);
	glGenRenderbuffers(2, &pingpongRBO[0]);
	
	for (GLuint i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		// Reset color buffer dimensions
		glBindTexture(GL_TEXTURE_2D, pingpongColorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		// attach it to currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorBuffer[i], 0);
		glBindRenderbuffer(GL_RENDERBUFFER, pingpongRBO[i]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, pingpongRBO[i]);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Logger::logError("Framebuffer is not complete!");
			success = false;
		}
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
	for (GLuint i = 0; i < 2; i++)
	{
		// Reset color buffer dimensions
		glBindTexture(GL_TEXTURE_2D, pingpongColorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
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

	// Then pingpong between color buffers creating a smaller texture every time
	while (texWidth > 1)
	{
		// first change texture size
		glBindTexture(GL_TEXTURE_2D, pingpongColorBuffer[change ? 1 : 0]);
		texWidth = texWidth / 2;
		texHeight = texHeight / 2;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth > 1 ? texWidth : 1, texHeight > 1 ? texHeight : 1, 0, GL_RGB, GL_FLOAT, NULL);

		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[change ? 1 : 0]);
		glViewport(0, 0, texWidth, texHeight);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, pingpongColorBuffer[change ? 0 : 1]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		change = !change;
	}

	// Once done read the luminescence value of 1x1 texture
	GLfloat luminescence[3];
	glReadPixels(0, 0, 1, 1, GL_RGB, GL_FLOAT, &luminescence);
	GLfloat lum = 0.2126 * luminescence[0] + 0.7152 * luminescence[1] + 0.0722 * luminescence[2];
	auto ret = lerp(prevExposure, 0.5 / lum, 0.05); // slowly adjust exposure based on average brightness
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
