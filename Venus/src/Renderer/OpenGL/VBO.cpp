#include "pch.h"
#include "VBO.h"

namespace Venus {

	VBO::VBO(GLfloat* vertices, GLsizeiptr size, GLenum usage)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, usage);
	}

	void VBO::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, ID);
	}

	void VBO::Unbind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VBO::Delete()
	{
		glDeleteBuffers(1, &ID);
	}
}