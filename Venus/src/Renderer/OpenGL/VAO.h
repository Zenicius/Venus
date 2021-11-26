#pragma once

#include <glad/glad.h>
#include "VBO.h"

namespace Venus {

	class VAO
	{
		public:
			VAO();

			void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
			void Bind() const;
			void Unbind() const;
			void Delete();

			void SetIndicesCount(int count);
			int GetIndicesCount() const { return m_IndicesCount; }

		private:
			GLuint ID;
			int m_IndicesCount;
	};

}

