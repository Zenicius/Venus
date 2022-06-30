#pragma once

#include "Renderer/Material.h"

namespace Venus {

	class MeshMaterial
	{
		public:
			MeshMaterial();
			~MeshMaterial();

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

		private:
			Ref<Material> m_Material;

			Ref<Texture2D> m_AlbedoMapTexture;
			Ref<Texture2D> m_NormalMapTexture;
			Ref<Texture2D> m_MetalnessMapTexture;
			Ref<Texture2D> m_RoughnessMapTexture;
	};
}

