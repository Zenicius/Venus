#pragma once

#include "Asset.h"

namespace Venus {

	inline static std::unordered_map<std::string, AssetType> s_AssetExtensionMap =
	{
		// Venus
		{ ".venus", AssetType::Scene },
		{ ".vsmat", AssetType::Material },

		// Models
		{ ".obj",	AssetType::Model },
		{ ".fbx",	AssetType::Model },
		{ ".gltf",	AssetType::Model },
		{ ".dae",	AssetType::Model },
		{ ".3ds",	AssetType::Model },

		// Textures
		{ ".png",	AssetType::Texture },
		{ ".jpg",	AssetType::Texture },
		{ ".jpeg",	AssetType::Texture },
		{ ".tga",	AssetType::Texture },

		// EnvMap
		{ ".hdr",	AssetType::EnvironmentMap },

		// Scripts
		{ ".cs",	AssetType::Script },

		// Font
		{ ".ttf",	AssetType::Font }
	};

}