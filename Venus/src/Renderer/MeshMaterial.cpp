#include "pch.h"
#include "MeshMaterial.h"

#include "Renderer/Renderer.h"
#include "Assets/AssetSerializer.h"

namespace Venus {

	static const std::string s_AlbedoColorUniform = "u_MaterialUniforms.AlbedoColor";
	static const std::string s_EmissionUniform = "u_MaterialUniforms.Emission";
	static const std::string s_UseNormalMapUniform = "u_MaterialUniforms.UseNormalMap";
	static const std::string s_MetalnessUniform = "u_MaterialUniforms.Metalness";
	static const std::string s_RoughnessUniform = "u_MaterialUniforms.Roughness";

	// Textures 
	static const std::string s_AlbedoMapUniform = "u_AlbedoTexture";
	static const std::string s_NormalMapUniform = "u_NormalTexture";
	static const std::string s_MetalnessMapUniform = "u_MetalnessTexture";
	static const std::string s_RoughnessMapUniform = "u_RoughnessTexture";

	static const uint32_t s_AlbedoMapBinding = 0;
	static const uint32_t s_NormalMapBinding = 1;
	static const uint32_t s_MetalnessMapBinding = 2;
	static const uint32_t s_RoughnessMapBinding = 3;

	/////////////////////////////////////////////////////////////////////////////
	// MESH MATERIAL ////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	Ref<MeshMaterial> MeshMaterial::Create(const std::string& name)
	{
		return CreateRef<MeshMaterial>(name);
	}

	MeshMaterial::MeshMaterial(const std::string& name)
		: m_Name(name)
	{
		m_Material = Material::Create(Renderer::GetShaderLibrary()->Get("PBR"));

		// Defaults
		SetAlbedoColor(glm::vec3(0.972f, 0.96f, 0.915f));
		SetEmission(0.0f);
		SetMetalness(0.0f);
		SetRoughtness(0.4f);
		SetUseNormalMap(false);

		ClearAlbedoMap();
		ClearNormalMap();
		ClearMetalnessMap();
		ClearRoughnessMap();
	}

	MeshMaterial::~MeshMaterial()
	{
	}

	void MeshMaterial::Bind()
	{
		m_Material->Bind();
	}

	glm::vec3& MeshMaterial::GetAlbedoColor()
	{
		return m_Material->GetFloat3(s_AlbedoColorUniform);
	}

	void MeshMaterial::SetAlbedoColor(const glm::vec3& color)
	{
		m_Material->SetFloat3(s_AlbedoColorUniform, color);
	}

	float& MeshMaterial::GetEmission()
	{
		return m_Material->GetFloat(s_EmissionUniform);
	}

	void MeshMaterial::SetEmission(float value)
	{
		m_Material->SetFloat(s_EmissionUniform, value);
	}

	void MeshMaterial::SetUseNormalMap(bool value)
	{
		m_Material->SetInt(s_UseNormalMapUniform, value);
	}

	int& MeshMaterial::IsUsingNormalMap()
	{
		return m_Material->GetInt(s_UseNormalMapUniform);
	}

	float& MeshMaterial::GetMetalness()
	{
		return m_Material->GetFloat(s_MetalnessUniform);
	}

	void MeshMaterial::SetMetalness(float value)
	{
		m_Material->SetFloat(s_MetalnessUniform, value);
	}

	float& MeshMaterial::GetRoughness()
	{
		return m_Material->GetFloat(s_RoughnessUniform);
	}

	void MeshMaterial::SetRoughtness(float value)
	{
		m_Material->SetFloat(s_RoughnessUniform, value);
	}

	Ref<Texture2D> MeshMaterial::GetAlbedoMap()
	{
		return m_AlbedoMapTexture;
	}

	void MeshMaterial::SetAlbedoMap(Ref<Texture2D> texture)
	{
		m_AlbedoMapTexture = texture;
		m_Material->SetTexture(s_AlbedoMapUniform, s_AlbedoMapBinding, texture->GetRendererID());
	}

	void MeshMaterial::ClearAlbedoMap()
	{
		m_AlbedoMapTexture = Renderer::GetDefaultTexture();
		m_Material->SetTexture(s_AlbedoMapUniform, s_AlbedoMapBinding, Renderer::GetDefaultTexture()->GetRendererID());
	}

	Ref<Texture2D> MeshMaterial::GetNormalMap()
	{
		return m_NormalMapTexture;
	}

	void MeshMaterial::SetNormalMap(Ref<Texture2D> texture)
	{
		m_NormalMapTexture = texture;
		m_Material->SetTexture(s_NormalMapUniform, s_NormalMapBinding, texture->GetRendererID());
	}

	void MeshMaterial::ClearNormalMap()
	{
		SetUseNormalMap(false);
		m_NormalMapTexture = Renderer::GetDefaultTexture();
		m_Material->SetTexture(s_NormalMapUniform, s_NormalMapBinding, Renderer::GetDefaultTexture()->GetRendererID());
	}

	Ref<Texture2D> MeshMaterial::GetMetalnessMap()
	{
		return m_MetalnessMapTexture;
	}

	void MeshMaterial::SetMetalnessMap(Ref<Texture2D> texture)
	{
		m_MetalnessMapTexture = texture;
		m_Material->SetTexture(s_MetalnessMapUniform, s_MetalnessMapBinding, texture->GetRendererID());
	}

	void MeshMaterial::ClearMetalnessMap()
	{
		m_MetalnessMapTexture = Renderer::GetDefaultTexture();
		m_Material->SetTexture(s_MetalnessMapUniform, s_MetalnessMapBinding, Renderer::GetDefaultTexture()->GetRendererID());
	}

	Ref<Texture2D> MeshMaterial::GetRoughnessMap()
	{
		return m_RoughnessMapTexture;
	}

	void MeshMaterial::SetRoughnessMap(Ref<Texture2D> texture)
	{
		m_RoughnessMapTexture = texture;
		m_Material->SetTexture(s_RoughnessMapUniform, s_RoughnessMapBinding, texture->GetRendererID());
	}

	void MeshMaterial::ClearRoughnessMap()
	{
		m_RoughnessMapTexture = Renderer::GetDefaultTexture();
		m_Material->SetTexture(s_RoughnessMapUniform, s_RoughnessMapBinding, Renderer::GetDefaultTexture()->GetRendererID());
	}

	/////////////////////////////////////////////////////////////////////////////
	// MATERIAL TABLE ///////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	MaterialTable::MaterialTable(uint32_t count)
		:m_MaterialCount(count)
	{
	}

	MaterialTable::MaterialTable(Ref<MaterialTable> other)
		:m_MaterialCount(other->m_MaterialCount)
	{
		const auto& materials = other->GetMaterials();
		for (auto [index, material] : materials)
			SetMaterial(index, material);
	}

	void MaterialTable::SetMaterial(uint32_t index, Ref<MeshMaterial> material)
	{
		m_Materials[index] = material;
		if (index >= m_MaterialCount)
			m_MaterialCount = index + 1;
	}

	void MaterialTable::ClearMaterial(uint32_t index)
	{
		VS_CORE_ASSERT(HasMaterial(index));
		m_Materials.erase(index);
		if (index >= m_MaterialCount)
			m_MaterialCount = index + 1;
	}
}