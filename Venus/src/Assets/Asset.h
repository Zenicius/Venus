#pragma once

#include "Engine/UUID.h"
#include "Engine/Base.h"

namespace Venus {

	using AssetHandle = UUID;
	
	enum class AssetFlag : uint16_t
	{
		None = 0,
		Missing = BIT(0),
		Invalid = BIT(1)
	};

	enum class AssetType : uint16_t
	{
		None = 0,
		Texture = 1,
		Font = 2,
		Scene = 3,
		Model = 4,
		Script = 5,
		EnvironmentMap = 6,
		Material = 7,
		Prefab = 8
	};

	namespace Utils {
	
		inline std::string AssetTypeToString(AssetType type)
		{
			switch (type)
			{
				case AssetType::None:			return "None";
				case AssetType::Texture:		return "Texture";
				case AssetType::Font:			return "Font";
				case AssetType::Scene:			return "Scene";
				case AssetType::Model:			return "Model";
				case AssetType::Script:			return "Script";
				case AssetType::EnvironmentMap: return "Environment Map";
				case AssetType::Material:		return "Material";
				case AssetType::Prefab:			return "Prefab";
			}

			VS_CORE_ASSERT(false, "Unknown Type");
			return "None";
		}

		inline AssetType AssetTypeFromString(const std::string& assetType)
		{
			if (assetType == "None")			return AssetType::None;
			if (assetType == "Texture")			return AssetType::Texture;
			if (assetType == "Font")			return AssetType::Font;
			if (assetType == "Scene")			return AssetType::Scene;
			if (assetType == "Model")			return AssetType::Model;
			if (assetType == "Script")			return AssetType::Script;
			if (assetType == "Environment Map")	return AssetType::EnvironmentMap;
			if (assetType == "Material")		return AssetType::Material;
			if (assetType == "Prefab")			return AssetType::Prefab;

			VS_CORE_ASSERT(false, "Unknown Type");
			return AssetType::None;
		}
	}

	struct AssetMetadata
	{
		AssetHandle Handle = 0;
		AssetType Type;

		std::filesystem::path FilePath;
		bool IsDataLoaded = false;

		bool IsValid() const { return Handle != 0; }
	};

	class Asset
	{
		public:
			AssetHandle Handle = 0;
			uint16_t Flags = (uint16_t)AssetFlag::None;

			virtual ~Asset() {}

			bool IsValid() const { return (Flags & (uint16_t)AssetFlag::Missing | (Flags & (uint16_t)AssetFlag::Invalid)) == 0; }

			static AssetType GetStaticType() { return AssetType::None; }
			virtual AssetType GetAssetType() const { return AssetType::None; }

			virtual bool operator==(const Asset& other) const
			{
				return Handle == other.Handle;
			}

			virtual bool operator!=(const Asset& other) const
			{
				return !(*this == other);
			}

			bool IsFlagSet(AssetFlag flag) const { return (uint16_t)flag & Flags; }
			void SetFlag(AssetFlag flag, bool value = true)
			{
				if (value)
					Flags |= (uint16_t)flag;
				else
					Flags &= ~(uint16_t)flag;
			}
	};

}