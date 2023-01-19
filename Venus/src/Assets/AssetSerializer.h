#pragma once

#include "Asset.h"
#include "Renderer/MeshMaterial.h"

namespace Venus {

	class AssetSerializer
	{
		public:
			virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const = 0;
			virtual bool TryLoadAsset(const AssetMetadata& metadata, Ref<Asset>& asset) const = 0;
	};

	class ModelSerializer : public AssetSerializer
	{
		public:
			virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override {};
			virtual bool TryLoadAsset(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

	class TextureSerializer : public AssetSerializer
	{
		public:
			virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override {};
			virtual bool TryLoadAsset(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

	class MaterialSerializer : public AssetSerializer
	{
		public:
			virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
			virtual bool TryLoadAsset(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

	class SceneAssetSerializer : public AssetSerializer
	{
		public:
			virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override;
			virtual bool TryLoadAsset(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

	class EnvironmentSerializer : public AssetSerializer
	{
		public:
			virtual void Serialize(const AssetMetadata& metadata, const Ref<Asset>& asset) const override {};
			virtual bool TryLoadAsset(const AssetMetadata& metadata, Ref<Asset>& asset) const override;
	};

}

