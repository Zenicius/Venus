#pragma once

#include "Renderer/Material.h"
#include <map>

namespace Venus {

	class MeshMaterial : public Asset
	{
		public:
			static Ref<MeshMaterial> Create(const std::string& name = "Default Material");

			MeshMaterial(const std::string& name = "Default Material");
			~MeshMaterial();

			void Bind();

			// Params
			glm::vec3& GetAlbedoColor();
			void SetAlbedoColor(const glm::vec3& color);

			float& GetEmission();
			void SetEmission(float value);

			int& IsUsingNormalMap();
			void SetUseNormalMap(bool value);

			float& GetMetalness();
			void SetMetalness(float value);

			float& GetRoughness();
			void SetRoughtness(float value);

			// Textures
			Ref<Texture2D> GetAlbedoMap();
			void SetAlbedoMap(Ref<Texture2D> texture);
			void ClearAlbedoMap();

			Ref<Texture2D> GetNormalMap();
			void SetNormalMap(Ref<Texture2D> texture);
			void ClearNormalMap();

			Ref<Texture2D> GetMetalnessMap();
			void SetMetalnessMap(Ref<Texture2D> texture);
			void ClearMetalnessMap();

			Ref<Texture2D> GetRoughnessMap();
			void SetRoughnessMap(Ref<Texture2D> texture);
			void ClearRoughnessMap();

			// Plain Material
			Ref<Material> GetMaterial() const { return m_Material; }

			std::string GetName() { return m_Name; }

			static AssetType GetStaticType() { return AssetType::Material; }
			virtual AssetType GetAssetType() const override { return GetStaticType(); }

		private:
			Ref<Material> m_Material;
			
			std::string m_Name;

			Ref<Texture2D> m_AlbedoMapTexture;
			Ref<Texture2D> m_NormalMapTexture;
			Ref<Texture2D> m_MetalnessMapTexture;
			Ref<Texture2D> m_RoughnessMapTexture;
	};

	class MaterialTable
	{
		public:
			MaterialTable(uint32_t count = 1);
			MaterialTable(Ref<MaterialTable> other);
			~MaterialTable() = default;

			bool HasMaterial(uint32_t index) const { return m_Materials.find(index) != m_Materials.end(); }
			void SetMaterial(uint32_t index, Ref<MeshMaterial> material);
			void ClearMaterial(uint32_t index);

			Ref<MeshMaterial> GetMaterial(uint32_t index) const
			{
				VS_CORE_ASSERT(HasMaterial(index), "Material not found!");
				return m_Materials.at(index);
			}

			std::map<uint32_t, Ref<MeshMaterial>>& GetMaterials() { return m_Materials; }
			const std::map<uint32_t, Ref<MeshMaterial>>& GetMaterials() const { return m_Materials; }

			uint32_t GetMaterialCount() const { return m_MaterialCount; }
			void SetMaterialCount(uint32_t count) { m_MaterialCount = count; }

		private:
			std::map<uint32_t, Ref<MeshMaterial>> m_Materials;
			uint32_t m_MaterialCount;
	};
}

