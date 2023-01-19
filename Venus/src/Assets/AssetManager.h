#pragma once

#include "Asset.h"
#include "AssetRegistry.h"
#include "AssetImporter.h"

namespace Venus {

	extern const std::filesystem::path g_AssetsPath;

	class AssetManager
	{
		public:
			static void Init();
			static void Shutdown();

			static void OnImGuiRender(bool& show);

			static const AssetHandle GetHandle(const std::filesystem::path& path);
			static const AssetMetadata& GetMetadata(AssetHandle handle);
			static const AssetMetadata& GetMetadata(const std::filesystem::path& path);

			static AssetHandle ImportAsset(const std::filesystem::path& path);
			
			static std::filesystem::path GetPath(AssetHandle handle);
			static std::filesystem::path GetRelativePath(const std::filesystem::path& path);
			static AssetType GetAssetType(const std::string& extension);
			static bool IsAssetHandleValid(AssetHandle handle) { return IsMemoryAsset(handle) || GetMetadata(handle).IsValid(); }
			static bool IsMemoryAsset(AssetHandle handle) { return s_MemoryAssets.find(handle) != s_MemoryAssets.end(); }

			//- Asset Management--------------------------------------------------------------------
			template<typename T, typename... Args>
			static Ref<T> CreateNewAsset(const std::string& fileName, const std::string& directory, Args&&... args)
			{
				static_assert(std::is_base_of<Asset, T>::value, "CreateNewAsset only works for classes derived from Asset");

				AssetMetadata metadata;
				metadata.Handle = UUID();
				if (directory.empty() || directory == ".")
					metadata.FilePath = fileName;
				else
					metadata.FilePath = AssetManager::GetRelativePath(directory + "/" + fileName);
				metadata.IsDataLoaded = true;
				metadata.Type = T::GetStaticType();

				// Try to find available name to asset (ex: 'Asset (02)')
				if (std::filesystem::exists(g_AssetsPath / metadata.FilePath))
				{
					bool foundAvailableName = false;
					int current = 1;
					while (!foundAvailableName)
					{
						std::string nextFileName = directory + "/" + metadata.FilePath.stem().string();

						if (current < 10)
							nextFileName += " (0" + std::to_string(current) + ")";
						else
							nextFileName += " (" + std::to_string(current) + ")";
						nextFileName += metadata.FilePath.extension().string();

						if (!std::filesystem::exists(g_AssetsPath / nextFileName))
						{
							foundAvailableName = true;
							metadata.FilePath = GetRelativePath(nextFileName);
							break;
						}

						current++;
					}
				}

				s_AssetRegistry[metadata.FilePath] = metadata;
				SerializeRegistry();

				Ref<T> asset = std::make_shared<T>(std::forward<Args>(args)...);
				asset->Handle = metadata.Handle;
				s_LoadedAssets[asset->Handle] = asset;
				AssetImporter::Serialize(metadata, asset);

				return asset;
			}

			template<typename T, typename... Args>
			static AssetHandle CreateMemoryOnlyAsset(Args&&... args)
			{
				static_assert(std::is_base_of<Asset, T>::value, "CreateMemoryOnlyAsset only works for classes derived from Asset");

				Ref<T> asset = CreateRef<T>(std::forward<Args>(args)...);
				asset->Handle = UUID();

				s_MemoryAssets[asset->Handle] = asset;
				return asset->Handle;
			}

			template<typename T>
			static Ref<T> GetAsset(AssetHandle handle)
			{
				if (IsMemoryAsset(handle))
				{
					Ref<Asset> asset = s_MemoryAssets[handle];
					return std::dynamic_pointer_cast<T>(asset);
				}

				auto& metadata = GetMetadata_Internal(handle);
				if (!metadata.IsValid())
					return nullptr;

				Ref<Asset> asset = nullptr;
				if (!metadata.IsDataLoaded)
				{
					metadata.IsDataLoaded = AssetImporter::TryLoadAsset(metadata, asset);
					if (!metadata.IsDataLoaded)
						return nullptr;

					s_LoadedAssets[handle] = asset;
				}
				else
				{
					asset = s_LoadedAssets[handle];
				}

				return std::dynamic_pointer_cast<T>(asset);
			}

			template<typename T>
			static Ref<T> GetAsset(const std::filesystem::path& path)
			{
				auto& metadata = GetMetadata(path);
				return GetAsset<T>(metadata.Handle);
			}
			//--------------------------------------------------------------------------------------


		private:
			static AssetMetadata& GetMetadata_Internal(AssetHandle handle);

			static void ReloadAssets();
			static void ProcessDirectory(const std::filesystem::path& path);

			static void SerializeRegistry();
			static void DeserializeRegistry();

			static void OnAssetMoved(AssetHandle asset, const std::filesystem::path& newFilePath);
			static void OnAssetRenamed(AssetHandle asset, const std::filesystem::path& newFilePath);
			static void OnAssetDeleted(AssetHandle asset);

		private:
			static std::unordered_map<AssetHandle, Ref<Asset>> s_LoadedAssets;
			static std::unordered_map<AssetHandle, Ref<Asset>> s_MemoryAssets;
			inline static AssetRegistry s_AssetRegistry;

			friend class AssetBrowserPanel;
	};

}

