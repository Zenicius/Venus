#pragma once

#include <Venus.h>
#include <filesystem>

#include "imgui/imgui.h"

namespace Venus {

	enum class AssetType
	{
		Texture,
		Font,
		Scene,
		Prefab,
		Other
	};

	class AssetBrowserPanel
	{
		public:
			AssetBrowserPanel();
			
			AssetType GetFileType(std::filesystem::path file);
			void SetContext(const Ref<Scene>& context);
			std::filesystem::path GetSelectedAsset() { return m_SelectedAsset; }
			void SetSelectedAsset(std::filesystem::path asset);

			// Assets Browser
			void OnAssetTreeRender(std::filesystem::path path);
			void OnAssetRender(std::filesystem::directory_entry asset, float size);
			void OnAssetsExplorerRender();

			// Asset Editor
			void OnAssetEditorRender();

			void OnImGuiRender();

		private:
			bool CheckDirectoryHasFolders(std::filesystem::path path);
			bool CheckDirectoryHasFileNamed(std::filesystem::path path, std::string name);

			void MoveFileFromPayload(const ImGuiPayload* payload, std::filesystem::path path);

			bool ExistsInAssetMap(std::filesystem::path path) { return m_AssetsMap.find(path.string()) != m_AssetsMap.end(); }

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

			Ref<Texture2D> m_ReturnIcon;
			Ref<Texture2D> m_FowardIcon;
			Ref<Texture2D> m_SearchIcon;
			Ref<Texture2D> m_SettingsIcon;
			Ref<Texture2D> m_FolderIcon;

			Ref<Texture2D> m_FileIcon;
			Ref<Texture2D> m_TextureIcon;
			Ref<Texture2D> m_SceneIcon;
			Ref<Texture2D> m_FontIcon;
			Ref<Texture2D> m_PrefabIcon;
	};

}

