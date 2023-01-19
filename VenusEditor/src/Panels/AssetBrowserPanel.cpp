#include "AssetBrowserPanel.h"

#include "Assets/AssetSerializer.h"
#include "Assets/AssetManager.h"

namespace Venus {

	// TODO Move to projects system
	extern const std::filesystem::path g_AssetsPath = "Assets";
	static const std::string s_IconsPath = "Resources/Icons/AssetBrowserPanel";

	/////////////////////////////////////////////////////////////////////////////
	// UTILS ////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	bool AssetBrowserPanel::CheckDirectoryHasFolders(const std::filesystem::path& path)
	{
		for (auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.is_directory())
				return true;
		}

		return false;
	}

	bool AssetBrowserPanel::CheckDirectoryHasFileNamed(const std::filesystem::path& path, const std::string& name)
	{
		for (auto& entry : std::filesystem::directory_iterator(path))
		{
			auto relativePath = std::filesystem::relative(entry, g_AssetsPath);
			std::string fileNameString = relativePath.string();

			if (fileNameString == name)
				return true;
		}

		return false;
	}

	std::string AssetBrowserPanel::GetAssetTypeDragDropString(AssetType type)
	{
		switch (type)
		{
			case AssetType::Texture:			return "ASSET_TEXTURE";
			case AssetType::Scene:				return "ASSET_SCENE";
			case AssetType::Font:				return "ASSET_FONT";
			case AssetType::Model:				return "ASSET_MODEL";
			case AssetType::Script:				return "ASSET_SCRIPT";
			case AssetType::EnvironmentMap:		return "ASSET_ENVMAP";
			case AssetType::Material:			return "ASSET_MATERIAL";
			case AssetType::None:				return "ASSET_OTHER";
		}
	}

	Ref<Texture2D> AssetBrowserPanel::GetAssetIcon(AssetType type, const std::filesystem::path& path)
	{
		switch (type)
		{
			case AssetType::Texture:			return m_TextureIcon;
			case AssetType::Font:				return m_FontIcon;
			case AssetType::Scene:				return m_SceneIcon;
			case AssetType::Model:				return m_ModelIcon;
			case AssetType::Script:				return m_ScriptIcon;
			case AssetType::EnvironmentMap:		return m_EnvMapIcon;
			case AssetType::Material:			return m_MaterialIcon;
			case AssetType::None:				return m_FileIcon;
		}

		return m_FileIcon;
	}

	const ImGuiPayload* AssetBrowserPanel::AssetDragDropTarget() const
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
				return payload;

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_SCENE"))
				return payload;

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_FONT"))
				return payload;

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MODEL"))
				return payload;

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_SCRIPT"))
				return payload;

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_ENVMAP"))
				return payload;

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MATERIAL"))
				return payload;

			else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_OTHER"))
				return payload;
		}

		return nullptr;
	}

	void AssetBrowserPanel::AssetDragDropSource(const std::filesystem::path& relativePath, AssetType type)
	{
		if (ImGui::BeginDragDropSource())
		{
			std::filesystem::path path = g_AssetsPath / relativePath;
			Ref<Texture2D> icon = std::filesystem::is_directory(path) ? m_FolderIcon : GetAssetIcon(type, path);

			ImGui::Image((ImTextureID)icon->GetRendererID(), { 45, 45 }, { 0, 1 }, { 1, 0 });

			const wchar_t* filePath = relativePath.c_str();
			std::string typeString = GetAssetTypeDragDropString(type);
			ImGui::SetDragDropPayload(typeString.c_str(), filePath, (wcslen(filePath) + 1) * sizeof(wchar_t));

			ImGui::EndDragDropSource();
		}
	}

	void AssetBrowserPanel::MoveFileFromPayload(const ImGuiPayload* payload, const std::filesystem::path& dest)
	{
		const wchar_t* payloadPath = (const wchar_t*)payload->Data;

		std::filesystem::path path = payloadPath;
		std::string name = path.filename().string();

		AssetMetadata assetMetada = AssetManager::GetMetadata(path);

		std::filesystem::path from = std::filesystem::path(g_AssetsPath / std::filesystem::path(payloadPath));
		std::filesystem::path destFinal = std::filesystem::path(dest / std::filesystem::path(name));

		if (destFinal.parent_path() != from)
		{
			std::filesystem::rename(from, destFinal);

			if (assetMetada.IsValid())
			{
				AssetManager::OnAssetMoved(assetMetada.Handle, AssetManager::GetRelativePath(destFinal));
			}
		}
	}

	Ref<Texture2D> AssetBrowserPanel::GetTexturePreview(const std::filesystem::path& path)
	{
		return nullptr;
	}

	/////////////////////////////////////////////////////////////////////////////
	// PANEL ////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	AssetBrowserPanel::AssetBrowserPanel()
		:m_CurrentPath(g_AssetsPath)
	{
		TextureProperties props;
		props.Filter = TextureFilterMode::Bilinear;
		props.UseMipmaps = true;
		m_FolderIcon = Texture2D::Create(s_IconsPath + "/folder.png");
		m_TextureIcon = Texture2D::Create(s_IconsPath + "/texture.png", props);
		m_FileIcon = Texture2D::Create(s_IconsPath + "/file.png", props);
		m_SceneIcon = Texture2D::Create(s_IconsPath + "/scene.png", props);
		m_FontIcon = Texture2D::Create(s_IconsPath + "/font.png", props);
		m_ModelIcon = Texture2D::Create(s_IconsPath + "/model.png", props);
		m_ScriptIcon = Texture2D::Create(s_IconsPath + "/script.png", props);
		m_EnvMapIcon = Texture2D::Create(s_IconsPath + "/hdr.png", props);
		m_MaterialIcon = Texture2D::Create(s_IconsPath + "/material.png", props);
	}

	void AssetBrowserPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void AssetBrowserPanel::SetSelectedAsset(const std::filesystem::path& asset)
	{
		m_SelectedAsset = asset;
	}

	void AssetBrowserPanel::OnAssetTreeRender(const std::filesystem::path& path)
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
				else if (m_CurrentPath == g_AssetsPath)
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
				auto relativePath = std::filesystem::relative(path, g_AssetsPath);
				std::string fileNameString = relativePath.stem().string();

				std::string finalName = folderIcon + " " + fileNameString;
				bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)TreeID, base_flags, finalName.c_str());

				// Clicked
				if (ImGui::IsItemClicked())
				{
					m_SelectedInTree = TreeID;
					m_CurrentPath = path;
				}

				// Drag and Drop
				if(const ImGuiPayload* payload = AssetDragDropTarget())
					MoveFileFromPayload(payload, entry.path());

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
		auto relativePath = std::filesystem::relative(path, g_AssetsPath);
		std::string fileNameString = relativePath.stem().string();

		AssetType type = s_AssetExtensionMap[path.extension().string()];

		// Not Asset or folder
		if (!asset.is_directory() && type == AssetType::None)
			return;

		// Unique ID
		ImGui::PushID(relativePath.filename().string().c_str());

		// Styling
		ImVec4 color, hoveredColor;

		if (m_SelectedAsset == path)
			color = { 0.30f, 0.25f, 0.25f, 1.0f };
		else
			color = { 0.0f, 0.0f, 0.0f, 0.0f };

		hoveredColor = { 0.2f, 0.2f, 0.2f, 1.0f };

		ImGui::PushStyleColor(ImGuiCol_Button, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoveredColor);

		// Get Asset Icon
		Ref<Texture2D> icon = std::filesystem::is_directory(path) ? m_FolderIcon : GetAssetIcon(type, path);

		// File/Folder button
		ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { size, size }, { 0, 1 }, { 1, 0 });

		// Options
		bool deleteAsset = false;
		if (ImGui::BeginPopupContextItem(relativePath.filename().string().c_str()))
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

				if (ImGui::MenuItem("Scene"))
				{
					if (asset.is_directory())
					{
						m_FowardPath = "";
						m_CurrentPath /= path.filename();
					}

					CreateAsset<Scene>("New Scene.venus");
				}

				if (ImGui::MenuItem("Material"))
				{
					if (asset.is_directory())
					{
						m_FowardPath = "";
						m_CurrentPath /= path.filename();
					}

					CreateAsset<MeshMaterial>("New Material.vsmat");
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();
			
			if (ImGui::MenuItem("Open"))
			{
				if (asset.is_directory())
					m_CurrentPath /= path.filename();
				else
					m_AssetOpenCallback(type, relativePath);
			}

			if (ImGui::MenuItem("Open Externally"))
				FileDialogs::Open(asset.path().string().c_str());

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
				{
					CORE_LOG_WARN("Deleting {0}", path.string());
					AssetHandle handle = AssetManager::GetHandle(relativePath);
					AssetManager::OnAssetDeleted(handle);
				}
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
		AssetDragDropSource(relativePath, type);
		// Target Folder
		if (asset.is_directory())
		{
			if (const ImGuiPayload* payload = AssetDragDropTarget())
				MoveFileFromPayload(payload, asset.path());
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
			strcpy_s(nameBuffer, sizeof(nameBuffer), path.stem().string().c_str());
			if (ImGui::InputText("##FileName", nameBuffer, sizeof(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				std::string fileName = nameBuffer;
				if (!fileName.empty())
				{
					std::string fullName = fileName + path.extension().string();
					std::string parentPath = asset.path().parent_path().string();
					std::string renamed = parentPath + "\\" + fullName;
					
					if (!CheckDirectoryHasFileNamed(parentPath, fullName))
					{
						std::filesystem::rename(path, renamed);

						AssetHandle handle = AssetManager::GetHandle(relativePath);
						AssetManager::OnAssetRenamed(handle, renamed);
					}
					else
						CORE_LOG_ERROR("Tried to rename {0} to {1} when name is in use!", m_RenamingAsset.string(), renamed);
				}

				m_RenamingAsset = "";
			}
			ImGui::SetKeyboardFocusHere(1);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 0.5f });
			ImGui::TextWrapped(fileNameString.c_str());
			ImGui::PopStyleColor();
		}

		ImGui::PopStyleColor(2);
		ImGui::NextColumn();
		ImGui::PopID();
	}

	void AssetBrowserPanel::OnAssetsExplorerRender()
	{
		ImGui::BeginChild("AssetsExplorer");

		// Return button
		if (ImGui::Button(ICON_FA_ARROW_LEFT))
		{
			if (m_CurrentPath != std::filesystem::path(g_AssetsPath))
			{
				m_FowardPath = m_CurrentPath;
				m_CurrentPath = m_CurrentPath.parent_path();
			}
		}
		UI::SetTooltip("Previous Directory");
		ImGui::SameLine();
		// Foward Button
		if (ImGui::Button(ICON_FA_ARROW_RIGHT))
		{
			if (!m_FowardPath.empty())
				m_CurrentPath = m_FowardPath;
		}
		UI::SetTooltip("Next Directory");

		// Current Path
		ImGui::PushStyleColor(ImGuiCol_Text, { 1.0, 1.0, 1.0, 0.5 });

		std::vector<std::filesystem::path> paths;
		std::filesystem::path currentPath = m_CurrentPath;
		while (currentPath != g_AssetsPath)
		{
			paths.push_back(currentPath);
			currentPath = currentPath.parent_path();
		}
		std::reverse(paths.begin(), paths.end());

		ImGui::SameLine();
		UI::ShiftPosX(10.0f);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
		ImVec2 textSize = ImGui::CalcTextSize(g_AssetsPath.string().c_str());
		if (ImGui::Selectable(g_AssetsPath.string().c_str(), false, 0, textSize))
			m_CurrentPath = g_AssetsPath;
		ImGui::PopFont();

		for (auto& path : paths)
		{
			ImGui::SameLine();
			ImGui::Text("/");
			UI::ShiftPosX(10.0f);
			ImGui::SameLine();

			std::string pathName = path.filename().string();

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
			ImVec2 textSize = ImGui::CalcTextSize(pathName.c_str());
			if (ImGui::Selectable(pathName.c_str(), false, 0, textSize))
				m_CurrentPath = path;
			ImGui::PopFont();
		}
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

		// Asset Items 
		ImGui::BeginChild("AssetItems");
		{
			UI::ShiftPosY(5.0f);

			// Deselect asset by clicking blank spaces
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

					if (ImGui::MenuItem("Scene"))
						CreateAsset<Scene>("New Scene.venus");

					if (ImGui::MenuItem("Material"))
						CreateAsset<MeshMaterial>("New Material.vsmat");

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Show in Explorer"))
					FileDialogs::Open(m_CurrentPath.string().c_str());

				ImGui::EndPopup();
			}

			// Columns Configuration
			float padding = 50.0f;
			float thumbnailSize = 75.0f;
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
		}
		ImGui::EndChild();

		ImGui::EndChild();
	}

	void AssetBrowserPanel::OnImGuiRender(bool& show)
	{
		if (!show)
			return;

		ImGui::Begin(ICON_FA_STAR " Asset Browser", &show, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

		ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable
			| ImGuiTableFlags_SizingFixedFit
			| ImGuiTableFlags_BordersInnerV;

		if (ImGui::BeginTable("AssetBrowserTable", 2, tableFlags))
		{
			ImGui::TableSetupColumn("Tree", 0, 325.0f);
			ImGui::TableSetupColumn("Items", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			
			ImGui::BeginChild("AssetTree");
			{
				// Assets Tree
				ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

				if (m_SelectedInTree == -1)
					base_flags |= ImGuiTreeNodeFlags_Selected;

				ImGui::SetNextItemOpen(m_RootTreeOpen);
				m_RootTreeOpen = ImGui::TreeNodeEx((void*)std::hash<std::string>()("Asset Root"), base_flags, ICON_FA_FOLDER_O " Assets");
			
				// Drag and Drop
				if(const ImGuiPayload* payload = AssetDragDropTarget())
					MoveFileFromPayload(payload, g_AssetsPath);

				// Render Tree
				if (m_RootTreeOpen)
				{
					if (ImGui::IsItemClicked())
					{
						m_SelectedInTree = -1;
						m_CurrentPath = g_AssetsPath;
					}

					OnAssetTreeRender(g_AssetsPath);
					ImGui::TreePop();
				}
			}
			ImGui::EndChild();

			ImGui::TableSetColumnIndex(1);

			OnAssetsExplorerRender();

			ImGui::EndTable();
		}

		ImGui::End();
	}
}