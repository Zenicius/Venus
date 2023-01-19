#pragma once

#include "AssetSerializer.h"

namespace Venus {

	class AssetImporter
	{
		public:
			static void Init();
			static void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset);
			static void Serialize(const Ref<Asset>& asset);
			static bool TryLoadAsset(const AssetMetadata& metadata, Ref<Asset>& asset);

		private:
			static std::unordered_map<AssetType, Scope<AssetSerializer>> s_Serializers;
	};

}

