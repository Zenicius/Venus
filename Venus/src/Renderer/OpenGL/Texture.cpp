#include "pch.h"
#include "Texture.h"

namespace Venus {

	Texture::Texture(const std::string& image, GLenum type, GLenum slot, GLenum format, GLenum pixelType, GLuint filter)
	{
		ID = -1;

		Type = type;
		int widthImg, heightImg, numColCh;

		stbi_set_flip_vertically_on_load(true);
		unsigned char* bytes = stbi_load(image.c_str(), &widthImg, &heightImg, &numColCh, 0);

		if (!bytes)
		{
			CORE_LOG_ERROR("Failed to load Texture : {0}", image);
			return;
		}

		glGenTextures(1, &ID);
		glActiveTexture(slot);
		glBindTexture(type, ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, format, widthImg, heightImg, 0, format, pixelType, bytes);
		glGenerateMipmap(type);

		stbi_image_free(bytes);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
	{
		GLuint texUni = glGetUniformLocation(shader.GetID(), uniform);
		glUniform1i(texUni, unit);
	}

	void Texture::Bind()
	{
		glBindTexture(Type, ID);
	}

	void Texture::Unbind()
	{
		glBindTexture(Type, ID);
	}

	void Texture::Delete()
	{
		glDeleteTextures(1, &ID);
	}
}