#include "pch.h"
#include "AssetImporter.h"

#include "AssetManager.h"

namespace Venus {

	std::unordered_map<AssetType, Scope<AssetSerializer>> AssetImporter::s_Serializers;

	void AssetImporter::Init()
	{
		s_Serializers[AssetType::Model] = CreateScope<ModelSerializer>();
		s_Serializers[AssetType::Texture] = CreateScope<TextureSerializer>();
		s_Serializers[AssetType::Material] = CreateScope<MaterialSerializer>();
		s_Serializers[AssetType::Scene] = CreateScope<SceneAssetSerializer>();
		s_Serializers[AssetType::EnvironmentMap] = CreateScope<EnvironmentSerializer>();
	}

	void AssetImporter::Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset)
	{
		if (s_Serializers.find(metadata.Type) == s_Serializers.end())
		{
			CORE_LOG_ERROR("There's no importer for assets of type: {0}", metadata.FilePath.extension().string());
			return;
		}

		s_Serializers[metadata.Type]->Serialize(metadata, asset);
	}

	void AssetImporter::Serialize(const Ref<Asset>& asset)
	{
		const AssetMetadata& metadata = AssetManager::GetMetadata(asset->Handle);
		Serialize(metadata, asset);
	}

	bool AssetImporter::TryLoadAsset(const AssetMetadata& metadata, Ref<Asset>& asset)
	{
		if (s_Serializers.find(metadata.Type) == s_Serializers.end())
		{
			CORE_LOG_ERROR("There's no importer for assets of type: {0}", metadata.FilePath.extension().string());
			return false;
		}

		return s_Serializers[metadata.Type]->TryLoadAsset(metadata, asset);
	}



}
