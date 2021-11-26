#pragma once

#include "stb_image.h"
#include "glad/glad.h"
#include "Shader.h"

namespace Venus {

	class Texture
	{
		public:
			Texture() {}
			Texture(const std::string& image, GLenum type, GLenum slot, GLenum format, GLenum pixelType, GLuint filter);

			void texUnit(Shader& shader, const char* uniform, GLuint unit);
			void Bind();
			void Unbind();
			void Delete();

		private:
			GLuint ID;
			GLenum Type;
	};

}

