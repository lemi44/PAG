#include "Postprocess.h"
#include "Logger.h"
#include <vector>


Postprocess::Postprocess()
{
}


Postprocess::~Postprocess()
{
	glDeleteFramebuffers(1, &framebuffer);
}

void Postprocess::updateTexture(const GLsizei width, const GLsizei height) const
{
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

bool Postprocess::init(const GLsizei width, const GLsizei height, Shader* shader)
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

	screenShader = shader;

	return success;
}

void Postprocess::update(const GLsizei width, const GLsizei height) const
{
	updateTexture(width, height);
	updateRenderObject(width, height);
}

void Postprocess::render(float exposure) const
{
	screenShader->use();
	screenShader->setFloat("exposure", exposure);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
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
