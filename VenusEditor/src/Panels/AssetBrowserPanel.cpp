#include "AssetBrowserPanel.h"

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
		m_FowardIcon = Texture2D::Create(s_IconsPath + "/foward.png");
		m_SearchIcon = Texture2D::Create(s_IconsPath + "/search.png");
		m_SettingsIcon = Texture2D::Create(s_IconsPath + "/settings.png");
		m_TextureIcon = Texture2D::Create(s_IconsPath + "/texture.png");
		m_SceneIcon = Texture2D::Create(s_IconsPath + "/scene.png");
		m_FontIcon = Texture2D::Create(s_IconsPath + "/font.png");
		m_ModelIcon = Texture2D::Create(s_IconsPath + "/model.png");
		m_ScriptIcon = Texture2D::Create(s_IconsPath + "/script.png");
	}

	void AssetBrowserPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void AssetBrowserPanel::SetSelectedAsset(std::filesystem::path asset)
	{
		m_SelectedAsset = asset;
	}

	void AssetBrowserPanel::OnAssetTreeRender(std::filesystem::path path)
	{	
		for (auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
			{
				// Uniqued ID for every tree
				int TreeID;
				if (!ExistsInAssetMap(entry))
				{
					m_AssetsMap[entry.path().string()] = m_LastID + 1;
					TreeID = m_LastID + 1;
					m_LastID++;
				}
				else
					TreeID = m_AssetsMap[entry.path().string()];

				// Select tree if is Current path
				if (m_CurrentPath == entry)
					m_SelectedInTree = TreeID;
				else if (m_CurrentPath == g_AssetPath)
					m_SelectedInTree = -1;

				// Tree configurations
				ImGuiTreeNodeFlags base_flags;
				std::string folderIcon = ICON_FA_FOLDER;

				bool entryHasFolders = CheckDirectoryHasFolders(entry);

				if (entryHasFolders)
					base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
				else
					base_flags = ImGuiTreeNodeFlags_Leaf;

				if (m_SelectedInTree == TreeID)
				{
					base_flags |= ImGuiTreeNodeFlags_Selected;
					folderIcon = ICON_FA_FOLDER_OPEN;
				}

				// Tree
				const auto& path = entry.path();
				auto relativePath = std::filesystem::relative(path, g_AssetPath);
				std::string fileNameString = relativePath.stem().string();

				std::string finalName = folderIcon + " " + fileNameString;
				bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)TreeID, base_flags, finalName.c_str());

				// Clicked
				if (ImGui::IsItemClicked())
				{
					m_SelectedInTree = TreeID;
					m_CurrentPath = path;
				}

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
						MoveFileFromPayload(payload, entry.path());

					else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_SCENE"))
						MoveFileFromPayload(payload, entry.path());

					else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PREFAB"))
						MoveFileFromPayload(payload, entry.path());

					else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FONT"))
						MoveFileFromPayload(payload, entry.path());

					else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MODEL"))
						MoveFileFromPayload(payload, entry.path());

					else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_SCRIPT"))
						MoveFileFromPayload(payload, entry.path());

					else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_OTHER"))
						MoveFileFromPayload(payload, entry.path());
				}

				// Open or closed
				if (node_open && entryHasFolders)
				{
					OnAssetTreeRender(path);
					ImGui::TreePop();
				}
				else if (node_open)
					ImGui::TreePop();
			}
		}
	}

	void AssetBrowserPanel::OnAssetRender(std::filesystem::directory_entry asset, float size)
	{
		const auto& path = asset.path();
		auto relativePath = std::filesystem::relative(path, g_AssetPath);
		std::string fileNameString = relativePath.stem().string();

		// Unique ID
		ImGui::PushID(fileNameString.c_str());

		ImVec4 color, hoveredColor;

		if (m_SelectedAsset == path)
			color = { 0.30f, 0.25f, 0.25f, 1.0f };
		else
			color = { 0.0f, 0.0f, 0.0f, 0.0f };

		hoveredColor = { 0.2f, 0.2f, 0.2f, 1.0f };

		ImGui::PushStyleColor(ImGuiCol_Button, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);

		// File type
		AssetType fileType = GetFileType(asset);

		// Switch icon by file type
		Ref<Texture2D> icon;
		if (!asset.is_directory())
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
				case AssetType::Model:
					icon = m_ModelIcon;
					break;
				case AssetType::Script:
					icon = m_ScriptIcon;
					break;
				case AssetType::Other:
					icon = m_FileIcon;
					break;
			}
		}
		else
			icon = m_FolderIcon;

		// File/Folder button
		ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { size, size }, { 0, 1 }, { 1, 0 });

		// Options
		bool deleteAsset = false;
		if (ImGui::BeginPopupContextItem(fileNameString.c_str()))
		{
			m_SelectedAsset = path;

			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Folder"))
				{
					std::string currentPath = asset.is_directory() ? path.string() : m_CurrentPath.string();
					std::string finalPath = currentPath + "\\New Folder";
					std::filesystem::create_directory(finalPath);

					if(asset.is_directory())
						m_CurrentPath /= path.filename();

					m_RenamingAsset = finalPath; // Rename Asset after Creation
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Open"))
			{
				if (asset.is_directory())
					m_CurrentPath /= path.filename();
				else
					FileDialogs::Open(asset.path().string().c_str());
			}

			if (ImGui::MenuItem("Show in Explorer"))
				FileDialogs::Open(asset.path().parent_path().string().c_str());

			if (ImGui::MenuItem("Rename"))
				m_RenamingAsset = path;

			if (ImGui::MenuItem("Delete"))
				deleteAsset = true;

			ImGui::EndPopup();
		}

		// Delete Asset Modal
		if (deleteAsset)
		{
			ImGui::OpenPopup("Delete?");
			// Always center this window when appearing
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		}
		if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("The file will be deleted.\nThis operation cannot be undone!\n\n");
			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0))) 
			{
				if (!asset.is_directory() && std::filesystem::remove(path))
					CORE_LOG_WARN("Deleting {0}", path.string());
				else if (asset.is_directory() && std::filesystem::remove_all(path))
					CORE_LOG_WARN("Deleting all in {0}", path.string());

				ImGui::CloseCurrentPopup(); 
			}

			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
		}

		// Drag and Drop 
		// Source
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

				case AssetType::Font:
				{
					const wchar_t* filePath = relativePath.c_str();
					ImGui::SetDragDropPayload("ASSET_FONT", filePath, (wcslen(filePath) + 1) * sizeof(wchar_t));
					break;
				}

				case AssetType::Model:
				{
					const wchar_t* filePath = relativePath.c_str();
					ImGui::SetDragDropPayload("ASSET_MODEL", filePath, (wcslen(filePath) + 1) * sizeof(wchar_t));
					break;
				}

				case AssetType::Script:
				{
					const wchar_t* filePath = relativePath.c_str();
					ImGui::SetDragDropPayload("ASSET_SCRIPT", filePath, (wcslen(filePath) + 1) * sizeof(wchar_t));
					break;
				}

				case AssetType::Other:
				{
					const wchar_t* filePath = relativePath.c_str();
					ImGui::SetDragDropPayload("ASSET_OTHER", filePath, (wcslen(filePath) + 1) * sizeof(wchar_t));
					break;
				}
			}

			ImGui::EndDragDropSource();
		}
		// Target Folder
		if (asset.is_directory())
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
					MoveFileFromPayload(payload, asset.path());

				else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_SCENE"))
					MoveFileFromPayload(payload, asset.path());

				else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FONT"))
					MoveFileFromPayload(payload, asset.path());

				else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MODEL"))
					MoveFileFromPayload(payload, asset.path());

				else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_SCRIPT"))
					MoveFileFromPayload(payload, asset.path());

				else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_OTHER"))
					MoveFileFromPayload(payload, asset.path());
			}
		}

		// Clicking
		if (ImGui::IsItemHovered())
		{
			// Select or Open Folder
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (asset.is_directory())
				{
					m_FowardPath = "";
					m_CurrentPath /= path.filename();
				}
			}
			else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_SelectedAsset = path;
			}
		}

		// Renaming or FileName
		if (m_RenamingAsset == path)
		{
			char nameBuffer[256];
			memset(nameBuffer, 0, sizeof(nameBuffer));
			strcpy_s(nameBuffer, sizeof(nameBuffer), path.filename().string().c_str());
			if (ImGui::InputText("##FileName", nameBuffer, sizeof(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				std::string fileName = nameBuffer;
				if (!fileName.empty())
				{
					std::string parentPath = asset.path().parent_path().string();
					std::string renamed = parentPath + "\\" + fileName;
					
					if (!CheckDirectoryHasFileNamed(parentPath, fileName))
						std::filesystem::rename(path, renamed);
					else
						CORE_LOG_ERROR("Tried to rename {0} to {1} when name is in use!", m_RenamingAsset.string(), renamed);
				}

				m_RenamingAsset = "";
			}
			ImGui::SetKeyboardFocusHere(1);
		}
		else
			ImGui::TextWrapped(fileNameString.c_str());

		ImGui::PopStyleColor(2);
		ImGui::NextColumn();
		ImGui::PopID();
	}

	void AssetBrowserPanel::OnAssetsExplorerRender()
	{
		ImGui::BeginChild("AssetsExplorer");

		// Deselect entity by clicking blank spaces
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_SelectedAsset = "";
		}

		// Options by right clicking
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Folder"))
				{
					std::string finalPath = m_CurrentPath.string() + "\\New Folder";
					std::filesystem::create_directory(finalPath);

					m_RenamingAsset = finalPath;
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Show in Explorer"))
				FileDialogs::Open(m_CurrentPath.string().c_str());

			ImGui::EndPopup();
		}

		// Return button
		if (ImGui::Button(ICON_FA_ARROW_LEFT))
		{
			if (m_CurrentPath != std::filesystem::path(g_AssetPath))
			{
				m_FowardPath = m_CurrentPath;
				m_CurrentPath = m_CurrentPath.parent_path();
			}
		}
		ImGui::SameLine();
		// Foward Button
		if (ImGui::Button(ICON_FA_ARROW_RIGHT))
		{
			if (!m_FowardPath.empty())
				m_CurrentPath = m_FowardPath;
		}
		// Current Path
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0, 1.0, 1.0, 0.5 });
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10.0f);
		ImGui::Text(m_CurrentPath.string().c_str());
		ImGui::PopStyleColor();

		// Search
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - 300.0f); //286
		ImGui::TextDisabled(ICON_FA_SEARCH);
		ImGui::SameLine();
		static ImGuiTextFilter assetFilter;
		assetFilter.Draw("##Search", 270.0f);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

		// Columns Configuration
		float padding = 40.0f;
		float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)panelWidth / cellSize;

		if (columnCount == 0)
			columnCount = 1;
		
		ImGui::Columns(columnCount, "AssetsExplorer", false);

		for (auto& entry : std::filesystem::directory_iterator(m_CurrentPath))
		{
			if (assetFilter.PassFilter(entry.path().string().c_str()))
				OnAssetRender(entry, thumbnailSize);

			ImGui::TableNextColumn();
		}

		ImGui::Columns(1);

		ImGui::EndChild();
	}

	void AssetBrowserPanel::OnImGuiRender(bool& show)
	{
		if (!show)
			return;

		ImGui::Begin(ICON_FA_STAR " Asset Browser", &show);

		ImGui::Columns(2, "outer", true);

		// Assets Tree
		ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		if (m_SelectedInTree == -1)
			base_flags |= ImGuiTreeNodeFlags_Selected;

		ImGui::SetNextItemOpen(m_RootTreeOpen);
		m_RootTreeOpen = ImGui::TreeNodeEx((void*)(intptr_t)-1, base_flags, ICON_FA_FOLDER_O " Assets");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
				MoveFileFromPayload(payload, g_AssetPath);

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_SCENE"))
				MoveFileFromPayload(payload, g_AssetPath);

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FONT"))
				MoveFileFromPayload(payload, g_AssetPath);

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MODEL"))
				MoveFileFromPayload(payload, g_AssetPath);

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_SCRIPT"))
				MoveFileFromPayload(payload, g_AssetPath);

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_OTHER"))
				MoveFileFromPayload(payload, g_AssetPath);
		}
		if (m_RootTreeOpen)
		{
			if (ImGui::IsItemClicked())
			{
				m_SelectedInTree = -1;
				m_CurrentPath = g_AssetPath;
			}
			
			OnAssetTreeRender(g_AssetPath);
			ImGui::TreePop();
		}

		ImGui::NextColumn();
		ImGui::SetColumnOffset(ImGui::GetColumnIndex(), 325);

		// Assets Explorer
		OnAssetsExplorerRender();

		ImGui::Columns(1);
	
		ImGui::End();
	}

	AssetType AssetBrowserPanel::GetFileType(std::filesystem::path file)
	{
		if (file.extension().string() == ".png" || file.extension().string() == ".jpg" || file.extension().string() == ".jpeg" || file.extension().string() == ".tga")
			return AssetType::Texture;

		else if (file.extension().string() == ".ttf")
			return AssetType::Font;

		else if (file.extension().string() == ".venus")
			return AssetType::Scene;

		else if (file.extension().string() == ".obj" || file.extension().string() == ".fbx" || file.extension().string() == ".gltf")
			return AssetType::Model;

		else if (file.extension().string() == ".cs" || file.extension().string() == ".glsl")
			return AssetType::Script;

		else
			return AssetType::Other;
	}

	bool AssetBrowserPanel::CheckDirectoryHasFolders(std::filesystem::path path)
	{
		for (auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
				return true;
		}

		return false;
	}

	bool AssetBrowserPanel::CheckDirectoryHasFileNamed(std::filesystem::path path, std::string name)
	{
		for (auto& entry : std::filesystem::directory_iterator(path))
		{
			auto relativePath = std::filesystem::relative(entry, g_AssetPath);
			std::string fileNameString = relativePath.string();

			if (fileNameString == name)
				return true;
		}

		return false;
	}

	void AssetBrowserPanel::MoveFileFromPayload(const ImGuiPayload* payload, std::filesystem::path path)
	{
		const wchar_t* payloadPath = (const wchar_t*)payload->Data;
		std::string name = std::filesystem::path(payloadPath).filename().string();

		std::filesystem::path from = std::filesystem::path(g_AssetPath / std::filesystem::path(payloadPath));
		std::filesystem::path dest = std::filesystem::path(path / std::filesystem::path(name));

		if(dest.parent_path() != from)
			std::filesystem::rename(from, dest);
	}

}