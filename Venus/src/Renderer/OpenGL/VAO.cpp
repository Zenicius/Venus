#include "pch.h"
#include "VAO.h"

namespace Venus {

	VAO::VAO(GLsizei size)
	{
		glGenVertexArrays(size, &ID);
	}

	void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
	{
		VBO.Bind();
		glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
		glEnableVertexAttribArray(layout);
		VBO.Unbind();
	}

	void VAO::Bind() const
	{
		glBindVertexArray(ID);
	}

	void VAO::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VAO::Delete()
	{
		glDeleteVertexArrays(1, &ID);
	}

	void VAO::SetIndicesCount(int count)
	{
		m_IndicesCount = count;
	}
}