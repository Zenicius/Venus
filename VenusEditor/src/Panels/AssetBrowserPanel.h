#pragma once

#include <Venus.h>
#include <filesystem>

namespace Venus {

	enum class AssetType
	{
		Texture,
		Font,
		Scene,
		Other
	};

	class AssetBrowserPanel
	{
		public:
			AssetBrowserPanel();
			
			void OnImGuiRender();
		
		private:
			AssetType GetFileType(std::filesystem::path file);

		private:
			std::filesystem::path m_CurrentPath;

			Ref<Texture2D> m_ReturnIcon;
			Ref<Texture2D> m_FolderIcon;

			Ref<Texture2D> m_FileIcon;
			Ref<Texture2D> m_TextureIcon;
			Ref<Texture2D> m_SceneIcon;
			Ref<Texture2D> m_FontIcon;
	};

}

