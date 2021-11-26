#pragma once

#include <glad/glad.h>

namespace Venus {

	class EBO
	{
		public:
			EBO(GLuint* indices, GLsizeiptr size);
			void Bind();
			void Unbind();
			void Delete();

		private:
			GLuint ID;
	};

}