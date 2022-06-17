#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

namespace Venus {

	enum class MaterialType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool, Texture, TextureCube, TextureArray
	};

	// TODO: use buffer as value
	struct MaterialUniform
	{
		MaterialType type = MaterialType::Int;
		std::string name = "";
		int intValue = 0;
		float floatValue = 0.0f;
		glm::vec2 float2Value = glm::vec2(0.0f);
		glm::vec3 float3Value = glm::vec3(0.0f);
		glm::vec4 float4Value = glm::vec4(0.0f);
		glm::mat4 mat4Value = glm::mat4(1.0f);
		uint32_t texture = 0;
	};

	class Material
	{
		public:
			static Ref<Material> Create(const Ref<Shader>& shader, const std::string& name = "");

			Material(const Ref<Shader>& shader, const std::string& name = "");
			~Material() = default;

			void Bind();

			void SetInt(const std::string& name, int value);
			void SetIntArray(const std::string& name, int* values, uint32_t count);
			void SetFloat(const std::string& name, float value);
			void SetFloat2(const std::string& name, const glm::vec2& value);
			void SetFloat3(const std::string& name, const glm::vec3& value);
			void SetFloat4(const std::string& name, const glm::vec4& value);
			void SetMat4(const std::string& name, const glm::mat4& value);

			int& GetInt(const std::string& name);
			float& GetFloat(const std::string& name);
			glm::vec3& GetFloat3(const std::string& name);

			void SetTexture(const std::string& name, int binding, uint32_t texture);
			uint32_t GetTexture(const std::string& name);

			void SetCubeMap(const std::string& name, int binding, uint32_t texture);
			void SetTextureArray(const std::string& name, int binding, uint32_t texture);

			bool Exists(const std::string& name);

		private:
			Ref<Shader> m_Shader;
			std::string m_Name;

			std::unordered_map<std::string, MaterialUniform> m_Uniforms;
	};

}
