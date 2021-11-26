#pragma once

#include "pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Venus {

	std::string ReadFile(const std::string& fileName);

	class Shader
	{
		public:
			Shader() {}
			Shader(const std::string& vertexFileName, const std::string& fragmentFileName);

			void Activate() const;
			void Delete() const;

			const GLuint GetID() const { return ID; }

			// Uniforms
			void SetUniform1i(const std::string& name, int value);
			void SetUniform1f(const std::string& name, float value);
			void SetUniform2f(const std::string& name, const glm::vec2& value);
			void SetUniform3f(const std::string& name, const glm::vec3& value);
			void SetUniform4f(const std::string& name, const glm::vec4& value);

			void SetUniformMat3(const std::string& name, const glm::mat3& matrix);
			void SetUniformMat4(const std::string& name, const glm::mat4& matrix);

		private:
			GLuint ID;
			mutable std::unordered_map<std::string, int> m_UniformLocationCache;

			void CompileErrors(GLuint shader, const std::string& type, const std::string& name);
			GLint GetUniformLocation(const std::string& name) const;
	};

}

