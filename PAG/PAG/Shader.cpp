#include "Shader.h"

Shader::Shader(std::string vert_path, std::string frag_path)
{
	programHandle = glCreateProgram();

	if (programHandle == 0)
	{
		fprintf(stderr, "Error creating program object.\n");
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
		fprintf(stderr, "Failed to link shader program!\n");

		GLint logLen;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLen);

		if (logLen > 0)
		{
			char* log = (char*)malloc(logLen);
			GLsizei written;
			glGetProgramInfoLog(programHandle, logLen, &written, log);

			fprintf(stderr, "Program log: \n%s", log);
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
		fprintf(stderr, "Could not open file %s", fileName.c_str());
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
void Shader::loadAndCompileShaderFromFile(GLint shaderType, std::string fileName, GLuint& programHandle)
{
	GLuint shaderObject = glCreateShader(shaderType);

	if (shaderObject == 0)
	{
		fprintf(stderr, "Error creating %s.\n", fileName.c_str());
		return;
	}

	std::string shaderCodeString = loadShader(fileName);

	if (shaderCodeString.empty())
	{
		printf("Shader code is empty! Shader name %s\n", fileName.c_str());
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
		fprintf(stderr, "%s compilation failed!\n", fileName.c_str());

		GLint logLen;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &logLen);

		if (logLen > 0)
		{
			char * log = (char *)malloc(logLen);

			GLsizei written;
			glGetShaderInfoLog(shaderObject, logLen, &written, log);

			fprintf(stderr, "Shader log: \n%s", log);
			free(log);
		}

		return;
	}

	glAttachShader(programHandle, shaderObject);
	glDeleteShader(shaderObject);
}
GLuint Shader::get()
{
	return programHandle;
}
