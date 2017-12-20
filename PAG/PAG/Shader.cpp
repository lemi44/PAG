#include "Shader.h"
#include <fstream>
#include "Logger.h"


Shader::Shader(const std::string vert_path, const std::string frag_path)
{
	programHandle = glCreateProgram();

	if (programHandle == 0)
	{
		Logger::logError(string_format("Error creating program object.\n"));
	}

	/* Shader load from file and compile */
	loadAndCompileShaderFromFile(GL_VERTEX_SHADER, vert_path, programHandle);
	loadAndCompileShaderFromFile(GL_FRAGMENT_SHADER, frag_path, programHandle);

	/* Link */
	glLinkProgram(programHandle);

	GLint status;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		Logger::logError(string_format("Failed to link shader program!\n"));

		GLint logLen;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLen);

		if (logLen > 0)
		{
			auto log = static_cast<char*>(malloc(logLen));
			GLsizei written;
			glGetProgramInfoLog(programHandle, logLen, &written, log);

			Logger::logError(string_format("Program log: \n%s", log));
			free(log);
		}
	}

}


Shader::~Shader()
{
}


/* Load sharder code from external file */
std::string Shader::loadShader(std::string fileName)
{
	std::string filetext;
	std::string line;
	std::ifstream inFile(fileName);

	if (!inFile)
	{
		Logger::logError(string_format("Could not open file %s", fileName.c_str()));
		inFile.close();

		return nullptr;
	}
	
	while (inFile.good())
	{
		getline(inFile, line);
		filetext.append(line + "\n");
	}

	inFile.close();

	return filetext;
}


/* Load and compile shader from the external file (uses loadShader(std::string) function) */
void Shader::loadAndCompileShaderFromFile(const GLint shaderType, std::string fileName, GLuint& programHandle)
{
	const auto shaderObject = glCreateShader(shaderType);

	if (shaderObject == 0)
	{
		Logger::logError(string_format("Error creating %s.", fileName.c_str()));
		return;
	}

	std::string shaderCodeString = loadShader(fileName);

	if (shaderCodeString.empty())
	{
		Logger::logWarning(string_format("Shader code is empty! Shader name %s", fileName.c_str()));
		return;
	}

	const char * shaderCode = shaderCodeString.c_str();
	const GLint codeSize = shaderCodeString.size();

	glShaderSource(shaderObject, 1, &shaderCode, &codeSize);
	glCompileShader(shaderObject);

	GLint result;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		Logger::logError(string_format("%s compilation failed!", fileName.c_str()));

		GLint logLen;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &logLen);

		if (logLen > 0)
		{
			auto log = static_cast<char *>(malloc(logLen));

			GLsizei written;
			glGetShaderInfoLog(shaderObject, logLen, &written, log);

			Logger::logError(string_format("Shader log: \n%s", log));
			free(log);
		}

		return;
	}

	glAttachShader(programHandle, shaderObject);
	glDeleteShader(shaderObject);
}
GLuint Shader::get() const
{
	return programHandle;
}
// activate the shader
// ------------------------------------------------------------------------
void Shader::use() const
{
	glUseProgram(programHandle);
}
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, const bool value) const
{
	glUniform1i(glGetUniformLocation(programHandle, name.c_str()), int(value));
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, const int value) const
{
	glUniform1i(glGetUniformLocation(programHandle, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, const float value) const
{
	glUniform1f(glGetUniformLocation(programHandle, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(programHandle, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, const float x, const float y) const
{
	glUniform2f(glGetUniformLocation(programHandle, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(programHandle, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, const float x, const float y, const float z) const
{
	glUniform3f(glGetUniformLocation(programHandle, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(programHandle, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, const float x, const float y, const float z, const float w) const
{
	glUniform4f(glGetUniformLocation(programHandle, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(programHandle, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(programHandle, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(programHandle, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}