#include "SSAO.h"
#include <random>
#include "Helpers.h"
#include "FullscreenQuad.h"
#include "Logger.h"

SSAO::SSAO()
{
}

bool SSAO::init(GLsizei width, GLsizei height, Shader* ssaoShader, Shader* ssaoShaderBlur)
{
	std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between 0.0 - 1.0
	std::default_random_engine generator;
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}
	std::vector<glm::vec3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f);
		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);


	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Logger::logError("Framebuffer is not complete!");
		return false;
	}

	shaderSSAO = ssaoShader;
	shaderSSAO->use();
	shaderSSAO->setInt("gPosition", 0);
	shaderSSAO->setInt("gNormal", 1);
	shaderSSAO->setInt("texNoise", 2);
	for (size_t i = 0; i < 64; i++)
		shaderSSAO->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
	shaderSSAO->setVec2("noiseScale", width / 4.0f, height / 4.0f);

	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glGenTextures(1, &ssaoColorBufferBlur);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glCheckError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Logger::logError("Framebuffer is not complete!");
		return false;
	}

	shaderSSAOBlur = ssaoShaderBlur;
	shaderSSAOBlur->use();
	shaderSSAOBlur->setInt("ssaoInput", 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void SSAO::render(Deferred* deferred, glm::mat4 projection)
{
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deferred->getGPosition());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deferred->getGNormal());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	shaderSSAO->use();
	shaderSSAO->setMat4("projection", projection);
	FullscreenQuad::renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	shaderSSAOBlur->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	FullscreenQuad::renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint SSAO::getSSAO() const
{
	return ssaoColorBufferBlur;
}


SSAO::~SSAO()
{
}
