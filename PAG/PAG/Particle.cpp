#include "Particle.h"
#include "glm/gtc/matrix_access.hpp"
#include "Logger.h"
#include "glm/gtx/norm.hpp"

ParticleGenerator::ParticleGenerator(Shader *shader, Texture texture, GLuint amount)
	: shader(shader), texture(texture), amount(amount)
{
	this->init();
}

void ParticleGenerator::Update(GLfloat dt, GraphNode &object, GLuint newParticles, glm::vec3 camera_pos, glm::vec3 offset)
{
	// Add new particles 
	for (GLuint i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], object, offset);
	}
	// Update all particles
	for (GLuint i = 0; i < this->amount; ++i)
	{
		Particle &p = this->particles[i];
		p.Life -= dt; // reduce life
		if (p.Life > 0.0f)
		{	// particle is alive, thus update
			p.Color.a -= dt * 2.5;
			p.Position += glm::vec3(0.0f, 0.5f, 0.0f) * dt;
			p.cameradistance = glm::length2(p.Position - camera_pos);
		}
		else p.cameradistance = -1.0f;
	}
	sortParticles();
}

// Render all particles
void ParticleGenerator::Draw(const ViewProjection vp, const glm::vec3 up, const glm::vec3 right)
{
	glEnable(GL_BLEND);
	glCheckError();
	// Use additive blending to give it a 'glow' effect
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCheckError();
	this->shader->use();
	this->shader->setMat4("view", vp.view);
	this->shader->setMat4("projection", vp.projection);
	this->shader->setVec3("camera_up", up);
	this->shader->setVec3("camera_right", right);
	for (Particle particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			this->shader->setVec3("offset", particle.Position);
			this->shader->setVec4("color", particle.Color);
			glActiveTexture(GL_TEXTURE0);
			glCheckError();
			this->texture.activate();
			glCheckError();
			glBindVertexArray(this->VAO);
			glCheckError();
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glCheckError();
			glBindVertexArray(0);
			glCheckError();
		}
	}
	// Don't forget to reset to default blending mode
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCheckError();
	glDisable(GL_BLEND);
	glCheckError();
}

void ParticleGenerator::init()
{
	// Set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), static_cast<GLvoid*>(nullptr));
	glBindVertexArray(0);

	// Create this->amount default particle instances
	for (GLuint i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle());
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, GraphNode &object, glm::vec3 camera_pos, glm::vec3 offset)
{
	GLfloat random = ((rand() % 100) - 50) / 300.0f;
	GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
	particle.Position = glm::vec3(glm::column(object.getTransform().getMatrix(), 3)) + random + offset;
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 1.0f;
	particle.cameradistance = glm::length2(particle.Position - camera_pos);
}
void ParticleGenerator::sortParticles()
{
	std::sort(this->particles.begin(), this->particles.end());
}