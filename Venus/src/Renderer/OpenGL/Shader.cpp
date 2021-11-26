#include "pch.h"
#include "Shader.h"

namespace Venus {

	std::string ReadFile(const std::string& fileName)
	{
		std::ifstream in(fileName, std::ios::binary);
		if (in)
		{
			std::string sourceCode;
			in.seekg(0, std::ios::end);
			sourceCode.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&sourceCode[0], sourceCode.size());
			in.close();
			return (sourceCode);
		}
		else
		{
			CORE_LOG_ERROR("Could not open Shader file: {0}", fileName);
			return "";
		}
	}

	GLint Shader::GetUniformLocation(const std::string& name) const
	{
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
			return m_UniformLocationCache[name];

		GLint location = glGetUniformLocation(ID, name.c_str());
		m_UniformLocationCache[name] = location;
		return location;
	}

	Shader::Shader(const std::string& vertexFileName, const std::string& fragmentFileName)
	{
		// Read shader source code from text file
		std::string vertexCode = ReadFile(vertexFileName);
		const char* vertexSource = vertexCode.c_str();
		std::string fragmentCode = ReadFile(fragmentFileName);
		const char* fragSource = fragmentCode.c_str();

		/*
		*
		* Create the vertex shader based on the source code and get its reference
		* Does the same to the fragment shader
		*
		*/
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		glCompileShader(vertexShader);
		CompileErrors(vertexShader, "Vertex", vertexFileName);

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragSource, NULL);
		glCompileShader(fragmentShader);
		CompileErrors(fragmentShader, "Fragment", fragmentFileName);

		// Create the shader program and get its reference
		ID = glCreateProgram();
		CompileErrors(ID, "Program", "Program");

		/*
		*
		* Attach the shaders to the program created
		* Link the program and the delete the shaders since they are already in the program
		*
		*/
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		glLinkProgram(ID);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void Shader::Activate() const
	{
		glUseProgram(ID);
	}

	void Shader::Delete() const
	{
		glDeleteProgram(ID);
	}

	void Shader::SetUniform1i(const std::string& name, int value)
	{
		GLint location = GetUniformLocation(name);
		glUniform1i(location, value);
	}

	void Shader::SetUniform1f(const std::string& name, float value)
	{
		GLint location = GetUniformLocation(name);
		glUniform1f(location, value);
	}

	void Shader::SetUniform2f(const std::string& name, const glm::vec2& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform2f(location, value.x, value.y);
	}

	void Shader::SetUniform3f(const std::string& name, const glm::vec3& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::SetUniform4f(const std::string& name, const glm::vec4& value)
	{
		GLint location = GetUniformLocation(name);
		glUniform4f(location, value.r, value.b, value.g, value.a);
	}

	void Shader::SetUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix3fv(location, 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
	}

	void Shader::CompileErrors(GLuint shader, const std::string& type, const std::string& name)
	{
		GLint isCompiled = 0;
		char infoLog[1024];

		if (type == "Program")
			glGetProgramiv(shader, GL_LINK_STATUS, &isCompiled);
		else
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

		if (isCompiled == GL_FALSE)
		{
			// TODO: FIX PROGRAM ALWAYS THROWING ERROR
			if (type == "Program")
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			else
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				CORE_LOG_ERROR("Failed to compile {0} Shader : {1} : {2}", type, name, infoLog);
			}
		}
	}


}