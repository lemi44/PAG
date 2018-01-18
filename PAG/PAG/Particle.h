#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"
#include <vector>
#include "GraphNode.h"

struct Particle {
	glm::vec3 Position;
	glm::vec4 Color;
	GLfloat Life;
	GLfloat cameradistance;
	Particle()
		: Position(0.0f), Color(1.0f), Life(0.0f) { }

	bool operator<(Particle& that) {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

class ParticleGenerator
{
public:
	// Constructor
	ParticleGenerator(Shader *shader, Texture texture, GLuint amount);
	// Update all particles
	void Update(GLfloat dt, GraphNode &object, GLuint newParticles, glm::vec3 camera_pos, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
	// Render all particles
	void Draw(ViewProjection vp, glm::vec3 up, glm::vec3 right);
private:
	// State
	std::vector<Particle> particles;
	GLuint amount;
	// Render state
	Shader *shader;
	Texture texture;
	GLuint VAO;
	// Initializes buffer and vertex attributes
	void init();
	// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	GLuint firstUnusedParticle();
	// Respawns particle
	void respawnParticle(Particle &particle, GraphNode &object, glm::vec3 camera_pos, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
	void sortParticles();
};