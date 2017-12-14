#pragma once
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <string>

class Shader
{
private:
	GLuint programHandle;

public:

	Shader(std::string vert_path = "Shaders/basic.vert", std::string frag_path = "Shaders/basic.frag");
	~Shader();
	std::string loadShader(std::string fileName);
	void loadAndCompileShaderFromFile(GLint shaderType, std::string fileName, GLuint& programHandle);
	GLuint get();
};

