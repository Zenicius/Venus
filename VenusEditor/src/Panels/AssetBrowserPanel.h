#pragma once

#include <Venus.h>
#include <filesystem>

#include "ImGui/UI.h"

namespace Venus {

	class AssetBrowserPanel
	{
		public:
			AssetBrowserPanel();
			
			void SetContext(const Ref<Scene>& context);
			void OnImGuiRender(bool& show);

			const std::filesystem::path& GetSelectedAsset() { return m_SelectedAsset; }
			void SetSelectedAsset(const std::filesystem::path& asset);

			void SetAssetOpenCallback(const std::function<void(AssetType, const std::filesystem::path&)>& func)
			{
				m_AssetOpenCallback = func;
			}

		private:
			// Assets Browser
			void OnAssetTreeRender(const std::filesystem::path& path);
			void OnAssetRender(std::filesystem::directory_entry asset, float size);
			void OnAssetsExplorerRender();

			// Utils
			template<typename T, typename... Args>
			Ref<T> CreateAsset(const std::string& name, Args&&... args)
			{
				Ref<T> asset = AssetManager::CreateNewAsset<T>(name, m_CurrentPath.string(), std::forward<Args>(args)...);
				if (!asset)
					return nullptr;
				
				std::filesystem::path createdPath = AssetManager::GetMetadata(asset->Handle).FilePath;
				m_RenamingAsset = g_AssetsPath / createdPath;

				return asset;
			}

			bool CheckDirectoryHasFolders(const std::filesystem::path& path);
			bool CheckDirectoryHasFileNamed(const std::filesystem::path& path, const std::string& name);
			std::string GetAssetTypeDragDropString(AssetType type);
			Ref<Texture2D> GetAssetIcon(AssetType type, const std::filesystem::path& path);

			const ImGuiPayload* AssetDragDropTarget() const;
			void AssetDragDropSource(const std::filesystem::path& relativePath, AssetType type);
			void MoveFileFromPayload(const ImGuiPayload* payload, const std::filesystem::path& dest);

			bool ExistsInAssetMap(const std::filesystem::path& path) { return m_AssetsMap.find(path.string()) != m_AssetsMap.end(); }
			Ref<Texture2D> GetTexturePreview(const std::filesystem::path& path);

		private:
			std::filesystem::path m_CurrentPath;
			std::filesystem::path m_FowardPath = "";
			std::filesystem::path m_SelectedAsset = "";
			std::filesystem::path m_RenamingAsset = "";

			std::unordered_map<std::string, int> m_AssetsMap;
			int m_SelectedInTree = -1;
			int m_LastID = -1;
			bool m_RootTreeOpen = true;

			Ref<Scene> m_Context;
			std::function<void(AssetType, const std::filesystem::path&)> m_AssetOpenCallback;

			Ref<Texture2D> m_FolderIcon;
			Ref<Texture2D> m_FileIcon;
			Ref<Texture2D> m_TextureIcon;
			Ref<Texture2D> m_SceneIcon;
			Ref<Texture2D> m_FontIcon;
		    Ref<Texture2D> m_ModelIcon;
		    Ref<Texture2D> m_ScriptIcon;
			Ref<Texture2D> m_EnvMapIcon;
			Ref<Texture2D> m_MaterialIcon;
	};

}

