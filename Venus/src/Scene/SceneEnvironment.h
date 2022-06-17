#pragma once

#include "Renderer/Texture.h"

namespace Venus {

	class SceneEnvironment
	{
		public:
			static Ref<SceneEnvironment> Create(const Ref<TextureCube>& radianceMap, const Ref<TextureCube>& irradianceMap);

			SceneEnvironment() = default;
			SceneEnvironment(const Ref<TextureCube>& radianceMap, const Ref<TextureCube>& irradianceMap)
				: m_RadianceMap(radianceMap), m_IrradianceMap(irradianceMap) {}

			Ref<TextureCube> GetRadianceMap() { return m_RadianceMap; }
			Ref<TextureCube> GetIrradianceMap() { return m_IrradianceMap; }

		private:
			Ref<TextureCube> m_RadianceMap;
			Ref<TextureCube> m_IrradianceMap;
	};

}

