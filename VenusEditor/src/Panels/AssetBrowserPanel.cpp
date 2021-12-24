#include "AssetBrowserPanel.h"

#include "imgui/imgui.h"

namespace Venus {

	// TODO Move to projects system
	extern const std::filesystem::path g_AssetPath = "assets";

	static const std::string s_IconsPath = "Resources/Icons/AssetBrowserPanel";

	AssetBrowserPanel::AssetBrowserPanel()
		:m_CurrentPath(g_AssetPath)
	{
		m_FolderIcon = Texture2D::Create(s_IconsPath + "/folder.png");
		m_FileIcon = Texture2D::Create(s_IconsPath + "/file.png");
		m_ReturnIcon = Texture2D::Create(s_IconsPath + "/return.png");
		m_TextureIcon = Texture2D::Create(s_IconsPath + "/texture.png");
		m_SceneIcon = Texture2D::Create(s_IconsPath + "/venus.png");
		m_FontIcon = Texture2D::Create(s_IconsPath + "/font.png");
	}

	void AssetBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Assets");
		
		// Options by right clicking
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Open in Explorer"))
				FileDialogs::OpenInExplorer(m_CurrentPath.string().c_str());

			ImGui::EndPopup();
		}

		// Return button
		if (ImGui::ImageButton((ImTextureID)m_ReturnIcon->GetRendererID(), { 24, 24 }))
		{
			if (m_CurrentPath != std::filesystem::path(g_AssetPath))
				m_CurrentPath = m_CurrentPath.parent_path();
		}

		// Columns Configuration
		float padding = 40.0f;
		float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)panelWidth / cellSize;

		ImGui::Columns(columnCount, 0, false);

		for (auto& entry : std::filesystem::directory_iterator(m_CurrentPath))
		{
			const auto& path = entry.path();
			auto relativePath = std::filesystem::relative(path, g_AssetPath);
			std::string fileNameString = relativePath.stem().string();

			// Unique ID
			ImGui::PushID(fileNameString.c_str());

			ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });

			// File type
			AssetType fileType = GetFileType(entry);

			// Switch icon by file type
			Ref<Texture2D> icon;
			if (!entry.is_directory())
			{
				switch (fileType)
				{
					case AssetType::Texture:
						icon = m_TextureIcon;
						break;
					case AssetType::Font:
						icon = m_FontIcon;
						break;
					case AssetType::Scene:
						icon = m_SceneIcon;
						break;
					case AssetType::Other:
						icon = m_FileIcon;
						break;
				}
			}
			else
				icon = m_FolderIcon;

			// File/Folder button
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			// Drag and Drop
			if (ImGui::BeginDragDropSource())
			{
				switch (fileType)
				{
					case AssetType::Scene:
					{
						const wchar_t* filePath = relativePath.c_str();
						ImGui::SetDragDropPayload("ASSET_SCENE", filePath, (wcslen(filePath) + 1) * sizeof(wchar_t));
						break;
					}

					case AssetType::Texture:
					{
						const wchar_t* filePath = relativePath.c_str();
						ImGui::SetDragDropPayload("ASSET_TEXTURE", filePath, (wcslen(filePath) + 1) * sizeof(wchar_t));
						break;
					}
					
				}

				ImGui::EndDragDropSource();
			}

			// Double Click
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (entry.is_directory())
				{
					m_CurrentPath /= path.filename();
				}
			}
			ImGui::TextWrapped(fileNameString.c_str());

			ImGui::PopStyleColor();
			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::End();
	}

	AssetType AssetBrowserPanel::GetFileType(std::filesystem::path file)
	{
		if (file.extension().string() == ".png" || file.extension().string() == ".jpg")
			return AssetType::Texture;

		else if (file.extension().string() == ".ttf")
			return AssetType::Font;

		else if (file.extension().string() == ".venus")
			return AssetType::Scene;

		else
			return AssetType::Other;
	}

}