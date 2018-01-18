#include "Texture.h"
#include "Logger.h"

using namespace std;

Texture::Texture()
{
}

bool Texture::loadTexture(const char* path, string directory)
{
	string filename = string(path);
	filename = directory + '/' + filename;
	bool success = false;
	// load and create a texture 
	// -------------------------
	glGenTextures(1, &id);
	//glActiveTexture(tex_number);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	float *data = stbi_loadf(filename.c_str(), &width, &height, &nrChannels, 4);
	if (data)
	{
		glBindTexture(GL_TEXTURE_2D, id); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
										   // set the texture wrapping parameters
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		success = true;
	}
	else
	{
		Logger::logError(string_format("Failed to load texture: %s",filename));
	}
	stbi_image_free(data);
	return success;
}

bool Texture::loadTexture(aiTexture *texture)
{
	bool success = true;
	// load and create a texture 
	// -------------------------
	glGenTextures(1, &id);
	//glActiveTexture(tex_number);
	// load image, create texture and generate mipmaps
	
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	
	if (texture->pcData)
	{
		bool wrapping = (texture->achFormatHint[0] & 0x01);
		
		glBindTexture(GL_TEXTURE_2D, id); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
										  // set the texture wrapping parameters
		if (texture->mHeight == 0)
		{
			int width, height, channels;
			float* data = stbi_loadf_from_memory(reinterpret_cast<stbi_uc*>(texture->pcData), texture->mWidth, &width, &height, &channels, 4);
			if (data)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data);
			else
				success = false;
			wrapping = true;
		}
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->mWidth, texture->mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture->pcData);
		if (success)
		{
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping ? GL_REPEAT : GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping ? GL_REPEAT : GL_CLAMP_TO_EDGE);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}
	else success = false;

	if (!success) Logger::logError("Failed to load embedded texture!");
	return success;
}

void Texture::white()
{
	glGenTextures(1, &id);

	GLubyte data[] = { 255, 255, 255, 255 };

	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void Texture::activate()
{
	glBindTexture(GL_TEXTURE_2D, id);
}

Texture::~Texture()
{
}
