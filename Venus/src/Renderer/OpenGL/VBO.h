#pragma once

#include <glad/glad.h>

namespace Venus {

	class VBO
	{
		public:
			VBO() {}
			VBO(GLfloat* vertices, GLsizeiptr size, GLenum usage);
			void Bind();
			void Unbind();
			void Delete();

		private:
			GLuint ID;
	};
}

