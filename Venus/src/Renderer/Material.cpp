#include "pch.h"
#include "Material.h"

#include "Renderer/RenderCommand.h"

namespace Venus {

	Ref<Material> Material::Create(const Ref<Shader>& shader, const std::string& name)
	{
		return CreateRef<Material>(shader, name);
	}

	Material::Material(const Ref<Shader>& shader, const std::string& name)
		: m_Shader(shader), m_Name(name)
	{
	}

	void Material::Bind()
	{
		for (auto& [name, uniform] : m_Uniforms)
		{
			switch (uniform.type)
			{
				case MaterialType::Int:
				{
					m_Shader->SetInt(uniform.name, uniform.intValue);
					break;
				}
				case MaterialType::Float:
				{
					m_Shader->SetFloat(uniform.name, uniform.floatValue);
					break;
				}
				case MaterialType::Float2:
				{
					m_Shader->SetFloat2(uniform.name, uniform.float2Value);
					break;
				}
				case MaterialType::Float3:
				{
					m_Shader->SetFloat3(uniform.name, uniform.float3Value);
					break;
				}
				case MaterialType::Float4:
				{
					m_Shader->SetFloat3(uniform.name, uniform.float4Value);
					break;
				}
				case MaterialType::Mat4:
				{
					m_Shader->SetMat4(uniform.name, uniform.mat4Value);
					break;
				}
				case MaterialType::Texture:
				{
					m_Shader->SetTexture(uniform.name, uniform.intValue, uniform.texture);
					break;
				}
				case MaterialType::TextureCube:
				{
					m_Shader->SetCubeMap(uniform.name, uniform.intValue, uniform.texture);
					break;
				}
				case MaterialType::TextureArray:
				{
					m_Shader->SetTextureArray(uniform.name, uniform.intValue, uniform.texture);
					break;
				}
			}
		}
	}

	void Material::SetInt(const std::string& name, int value)
	{
		MaterialUniform uniform;
		uniform.type = MaterialType::Int;
		uniform.name = name;
		uniform.intValue = value;

		m_Uniforms[name] = uniform;
	}

	void Material::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		VS_CORE_ASSERT(false, "Not implemented!");
	}

	void Material::SetFloat(const std::string& name, float value)
	{
		MaterialUniform uniform;
		uniform.type = MaterialType::Float;
		uniform.name = name;
		uniform.floatValue = value;

		m_Uniforms[name] = uniform;
	}

	void Material::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		MaterialUniform uniform;
		uniform.type = MaterialType::Float2;
		uniform.name = name;
		uniform.float2Value = value;

		m_Uniforms[name] = uniform;
	}

	void Material::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		MaterialUniform uniform;
		uniform.type = MaterialType::Float3;
		uniform.name = name;
		uniform.float3Value = value;

		m_Uniforms[name] = uniform;
	}

	void Material::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		MaterialUniform uniform;
		uniform.type = MaterialType::Float4;
		uniform.name = name;
		uniform.float4Value = value;

		m_Uniforms[name] = uniform;
	}

	void Material::SetMat4(const std::string& name, const glm::mat4& value)
	{
		MaterialUniform uniform;
		uniform.type = MaterialType::Mat4;
		uniform.name = name;
		uniform.mat4Value = value;

		m_Uniforms[name] = uniform;
	}

	int& Material::GetInt(const std::string& name)
	{
		VS_CORE_ASSERT(Exists(name), "Uniform not found!");
		return m_Uniforms[name].intValue;
	}

	float& Material::GetFloat(const std::string& name)
	{
		VS_CORE_ASSERT(Exists(name), "Uniform not found!");
		return m_Uniforms[name].floatValue;
	}

	glm::vec3& Material::GetFloat3(const std::string& name)
	{
		VS_CORE_ASSERT(Exists(name), "Uniform not found!");
		return m_Uniforms[name].float3Value;
	}

	void Material::SetTexture(const std::string& name, int binding, uint32_t texture)
	{
		MaterialUniform uniform;
		uniform.type = MaterialType::Texture;
		uniform.name = name;
		uniform.intValue = binding;
		uniform.texture = texture;

		m_Uniforms[name] = uniform;
	}

	uint32_t Material::GetTexture(const std::string& name)
	{
		VS_CORE_ASSERT(Exists(name), "Uniform not found!");
		return m_Uniforms[name].texture;
	}

	void Material::SetCubeMap(const std::string& name, int binding, uint32_t texture)
	{
		MaterialUniform uniform;
		uniform.type = MaterialType::TextureCube;
		uniform.name = name;
		uniform.intValue = binding;
		uniform.texture = texture;

		m_Uniforms[name] = uniform;
	}

	void Material::SetTextureArray(const std::string& name, int binding, uint32_t texture)
	{
		MaterialUniform uniform;
		uniform.type = MaterialType::TextureArray;
		uniform.name = name;
		uniform.intValue = binding;
		uniform.texture = texture;

		m_Uniforms[name] = uniform;
	}

	bool Material::Exists(const std::string& name)
	{
		return m_Uniforms.find(name) != m_Uniforms.end();
	}


}