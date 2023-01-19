#pragma once

#include "Renderer/Texture.h"

namespace Venus {

	class SceneEnvironment : public Asset
	{
		public:
			static Ref<SceneEnvironment> Create(const Ref<TextureCube>& radianceMap, const Ref<TextureCube>& irradianceMap);

			SceneEnvironment() = default;
			SceneEnvironment(const Ref<TextureCube>& radianceMap, const Ref<TextureCube>& irradianceMap)
				: m_RadianceMap(radianceMap), m_IrradianceMap(irradianceMap) {}

			Ref<TextureCube> GetRadianceMap() { return m_RadianceMap; }
			Ref<TextureCube> GetIrradianceMap() { return m_IrradianceMap; }

			void SetRadianceMap(const Ref<TextureCube>& radianceMap) { m_RadianceMap = radianceMap; }
			void SetIrradianceMap(const Ref<TextureCube>& irradianceMap) { m_IrradianceMap = irradianceMap; }

			static AssetType GetStaticType() { return AssetType::EnvironmentMap; }
			virtual AssetType GetAssetType() const override { return GetStaticType(); }

		private:
			Ref<TextureCube> m_RadianceMap;
			Ref<TextureCube> m_IrradianceMap;
	};

}

