#include "EditorLayer.h"

#include "GLFW/glfw3.h"
#include "ImGui/UI.h"
#include <ImGuizmo/ImGuizmo.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Venus {

	extern const std::filesystem::path g_AssetPath;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		// Scene
		m_EditorScene = CreateRef<Scene>();		
		m_ActiveScene = m_EditorScene;
		m_EditorCamera = EditorCamera(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

		m_SceneRenderer = CreateRef<SceneRenderer>(m_ActiveScene);

		// Panels
		m_ObjectsPanel.SetContext(m_ActiveScene);
		m_AssetBrowserPanel.SetContext(m_ActiveScene);

		// Icons 
		TextureProperties props;
		props.FlipVertically = false;
		m_VenusLogoIcon = Texture2D::Create("Resources/Icons/venus.png");
		m_PlayIcon = Texture2D::Create("Resources/Icons/Toolbar/play.png");
		m_StopIcon = Texture2D::Create("Resources/Icons/Toolbar/stop.png");
		m_EditorCameraIcon = Texture2D::Create("Resources/Icons/Toolbar/camera.png");
		m_SceneCameraIcon = Texture2D::Create("Resources/Icons/Scene/camera.png");
		m_CreateIcon = Texture2D::Create("Resources/Icons/Toolbar/create.png", props);
		m_SaveIcon = Texture2D::Create("Resources/Icons/Toolbar/save.png");
		m_UndoIcon = Texture2D::Create("Resources/Icons/Toolbar/undo.png");
		m_RedoIcon = Texture2D::Create("Resources/Icons/Toolbar/redo.png");
		m_SettingsIcon = Texture2D::Create("Resources/Icons/Toolbar/settings.png");
		m_MinimizeIcon = Texture2D::Create("Resources/Icons/Titlebar/minimize.png");
		m_MaximizeIcon = Texture2D::Create("Resources/Icons/Titlebar/maximize.png");
		m_RestoreIcon = Texture2D::Create("Resources/Icons/Titlebar/restore.png", props);
		m_CloseIcon = Texture2D::Create("Resources/Icons/Titlebar/close.png");

		UpdateWindowTitle(m_EditorScene->m_SceneName);
	}

	void EditorLayer::OnDetach() {}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		// Render
		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				m_EditorCamera.SetActive(m_ViewportHovered);
				m_EditorCamera.OnUpdate(ts);
				m_ActiveScene->OnUpdateEditor(m_SceneRenderer, ts, m_EditorCamera);
				break;
			}
			case SceneState::Play:
			{
				m_ActiveScene->OnUpdateRuntime(m_SceneRenderer, ts);
				break;
			}
		} 

		UpdateHoveredEntity();
	}

	void EditorLayer::OnImGuiRender()
	{
		VS_PROFILE_FUNCTION();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		ImGui::PopStyleColor(); // MenuBarBg
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Border and resize
		//bool maximized = Application::Get().GetWindow().IsMaximized();
		OnManualWindowResize();
		//UI_WindowBorder();
		

		// Title bar
		const float titlebarHeight = UI_TitleBar();
		ImGui::SetCursorPosY(titlebarHeight + ImGui::GetCurrentWindow()->WindowPadding.y);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowMinSize.x = 100.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = 64.0f;

		// UI
		UI_ModalWelcome();
		UI_ToolBar();
		UI_Viewport();
		UI_Settings();

		//Renderer::OnImGuiRender();

		if(m_ShowWelcomeMessage)
			ImGui::OpenPopup("Welcome");

		// Panels
		m_ObjectsPanel.OnImGuiRender();
		m_AssetBrowserPanel.OnImGuiRender(m_ShowAssetBrowserPanel);
		m_RendererStatsPanel.OnImGuiRender(m_ShowStatsPanel);
		m_SceneRenderer->OnImGuiRender(m_ShowSceneSettingsPanel);

		ImGui::End();
	}

	void EditorLayer::UI_ModalWelcome()
	{
		if (ImGui::BeginPopupModal("Welcome", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Welcome to Venus Engine!");
			ImGui::Text("This engine is currently in its initial stage of development.");
			ImGui::Text("It may contain bugs and led to loss of work.");
			ImGui::Text("But thank you for your interest in Venus!");
			ImGui::Separator();
			if (ImGui::Button("Close", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
			ImGui::EndPopup();
			m_ShowWelcomeMessage = false;
		}
	}

	float EditorLayer::UI_TitleBar()
	{
		const float titlebarHeight = 47.0f;
		const ImVec2 windowPadding = ImGui::GetCurrentWindow()->WindowPadding;

		ImGui::SetCursorPos(ImVec2(windowPadding.x, windowPadding.y));
		const ImVec2 titlebarMin = ImGui::GetCursorScreenPos();
		const ImVec2 titlebarMax = { ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth() - windowPadding.y * 2.0f,
									 ImGui::GetCursorScreenPos().y + titlebarHeight };
		auto* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(titlebarMin, titlebarMax, IM_COL32(21, 21, 21, 255));

		// Logo
		{
			const int logoWidth = m_VenusLogoIcon->GetWidth();
			const int logoHeight = m_VenusLogoIcon->GetHeight();
			const ImVec2 logoOffset(16.0f + windowPadding.x, 8.0f + windowPadding.y);
			const ImVec2 logoRectStart = { ImGui::GetItemRectMin().x + logoOffset.x, ImGui::GetItemRectMin().y + logoOffset.y };
			const ImVec2 logoRectMax = { logoRectStart.x + logoWidth, logoRectStart.y + logoHeight };
			drawList->AddImage((ImTextureID)m_VenusLogoIcon->GetRendererID(), logoRectStart, logoRectMax);
		}

		ImGui::BeginHorizontal("Titlebar", { ImGui::GetWindowWidth() - windowPadding.y * 2.0f, ImGui::GetFrameHeightWithSpacing() });

		static float moveOffsetX;
		static float moveOffsetY;
		const float w = ImGui::GetContentRegionAvail().x;
		const float buttonsAreaWidth = 190;

		// Drag Area
		ImGui::InvisibleButton("##titleBarDragZone", ImVec2(w - buttonsAreaWidth, titlebarHeight));
		m_TitleBarHovered = ImGui::IsItemHovered();

		// Menu bar
		ImGui::SuspendLayout();
		ImGui::SetItemAllowOverlap();
		const float logoOffset = 16.0f * 2.0f + 41.0f + windowPadding.x;
		ImGui::SetCursorPos(ImVec2(logoOffset, 10.0f));
		UI_MenuBar();
		if (ImGui::IsItemHovered())
			m_TitleBarHovered = false;
		
		// Scene Name
		const std::string title = m_ActiveScene->m_SceneName;
		const ImVec2 textSize = ImGui::CalcTextSize(title.c_str());
		const float rightOffset = ImGui::GetWindowWidth() / 2.0f;
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - rightOffset - textSize.x);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 20.0f);
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Bold
		ImGui::Text(title.c_str());
		ImGui::PopFont();

		ImGui::ResumeLayout();

		// Window Buttons
		const float buttonWidth = 14.0f;
		const float buttonHeight = 14.0f;
		{
			// Minimize
			uint32_t iconID = m_MinimizeIcon->GetRendererID();
			ImGui::Spring();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);
			const int iconWidth = m_MinimizeIcon->GetWidth();
			const int iconHeight = m_MinimizeIcon->GetHeight();
			const float padY = (buttonHeight - (float)iconHeight) / 2.0f;
			if (ImGui::InvisibleButton("Minimize", ImVec2(buttonWidth, buttonHeight)))
			{
				Application::Get().GetWindow().Minimize();
			}

			ImRect rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
			rect.Min.y -= -padY;
			rect.Max.y += -padY;

			auto activeColor = IM_COL32(250, 250, 250, 255);
			auto hoveredColor = IM_COL32(128, 128, 128, 255);
			auto normalColor = IM_COL32(200, 200, 200, 255);

			auto* drawList = ImGui::GetWindowDrawList();
			if (ImGui::IsItemActive())
				drawList->AddImage((ImTextureID)iconID, rect.Min, rect.Max, ImVec2(0, 0), ImVec2(1, 1), activeColor);
			else if (ImGui::IsItemHovered())
				drawList->AddImage((ImTextureID)iconID, rect.Min, rect.Max, ImVec2(0, 0), ImVec2(1, 1), hoveredColor);
			else
				drawList->AddImage((ImTextureID)iconID, rect.Min, rect.Max, ImVec2(0, 0), ImVec2(1, 1), normalColor);
		}
		{
			// Maximize
			auto& window = Application::Get().GetWindow();
			bool maximized = window.IsMaximized();
			uint32_t iconID = maximized ? m_RestoreIcon->GetRendererID() : m_MaximizeIcon->GetRendererID();
			ImGui::Spring();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 25.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);
			if (ImGui::InvisibleButton("Maximize", ImVec2(buttonWidth, buttonHeight)))
			{
				if (maximized)
					window.Restore();
				else
					window.Maximize();
			}

			ImRect rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

			auto activeColor = IM_COL32(250, 250, 250, 255);
			auto hoveredColor = IM_COL32(128, 128, 128, 255);
			auto normalColor = IM_COL32(200, 200, 200, 255);

			auto* drawList = ImGui::GetWindowDrawList();
			if (ImGui::IsItemActive())
				drawList->AddImage((ImTextureID)iconID, rect.Min, rect.Max, ImVec2(0, 0), ImVec2(1, 1), activeColor);
			else if (ImGui::IsItemHovered())
				drawList->AddImage((ImTextureID)iconID, rect.Min, rect.Max, ImVec2(0, 0), ImVec2(1, 1), hoveredColor);
			else
				drawList->AddImage((ImTextureID)iconID, rect.Min, rect.Max, ImVec2(0, 0), ImVec2(1, 1), normalColor);
		}
		{
			// Close
			uint32_t iconID = m_CloseIcon->GetRendererID();
			ImGui::Spring();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 40.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20.0f);
			if (ImGui::InvisibleButton("Close", ImVec2(buttonWidth, buttonHeight)))
			{
				Application::Get().Close();
			}

			ImRect rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

			auto activeColor = IM_COL32(250, 0, 0, 255);
			auto hoveredColor = IM_COL32(158, 30, 30, 255);
			auto normalColor = IM_COL32(200, 200, 200, 255);

			auto* drawList = ImGui::GetWindowDrawList();
			if (ImGui::IsItemActive())
				drawList->AddImage((ImTextureID)iconID, rect.Min, rect.Max, ImVec2(0, 0), ImVec2(1, 1), activeColor);
			else if (ImGui::IsItemHovered())
				drawList->AddImage((ImTextureID)iconID, rect.Min, rect.Max, ImVec2(0, 0), ImVec2(1, 1), hoveredColor);
			else
				drawList->AddImage((ImTextureID)iconID, rect.Min, rect.Max, ImVec2(0, 0), ImVec2(1, 1), normalColor);
		}

		ImGui::EndHorizontal();
		return titlebarHeight;
	}

	void EditorLayer::UI_MenuBar()
	{
		const ImRect menuBarRect = { ImGui::GetCursorPos(), {ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeightWithSpacing()} };
		bool MenuBar = false;

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;
		/*if (!(window->Flags & ImGuiWindowFlags_MenuBar))
			return false;*/

		IM_ASSERT(!window->DC.MenuBarAppending);
		ImGui::BeginGroup(); // Backup position on layer 0 // FIXME: Misleading to use a group for that backup/restore
		ImGui::PushID("##menubar");

		const ImVec2 padding = window->WindowPadding;

		// Rect Offset
		ImRect result = menuBarRect;
		result.Min.x += 0.0f;
		result.Min.y += padding.y;
		result.Max.x += 0.0f;
		result.Max.y += padding.y;

		// We don't clip with current window clipping rectangle as it is already set to the area below. However we clip with window full rect.
		// We remove 1 worth of rounding to Max.x to that text in long menus and small windows don't tend to display over the lower-right rounded area, which looks particularly glitchy.
		ImRect bar_rect = result;// window->MenuBarRect();
		ImRect clip_rect(IM_ROUND(ImMax(window->Pos.x, bar_rect.Min.x + window->WindowBorderSize + window->Pos.x - 10.0f)), IM_ROUND(bar_rect.Min.y + window->WindowBorderSize + window->Pos.y),
			IM_ROUND(ImMax(bar_rect.Min.x + window->Pos.x, bar_rect.Max.x - ImMax(window->WindowRounding, window->WindowBorderSize))), IM_ROUND(bar_rect.Max.y + window->Pos.y));

		clip_rect.ClipWith(window->OuterRectClipped);
		ImGui::PushClipRect(clip_rect.Min, clip_rect.Max, false);

		// We overwrite CursorMaxPos because BeginGroup sets it to CursorPos (essentially the .EmitItem hack in EndMenuBar() would need something analogous here, maybe a BeginGroupEx() with flags).
		window->DC.CursorPos = window->DC.CursorMaxPos = ImVec2(bar_rect.Min.x + window->Pos.x, bar_rect.Min.y + window->Pos.y);
		window->DC.LayoutType = ImGuiLayoutType_Horizontal;
		window->DC.NavLayerCurrent = ImGuiNavLayer_Menu;
		window->DC.MenuBarAppending = true;
		ImGui::AlignTextToFramePadding();
		MenuBar =  true;


		ImGui::BeginGroup();

		if (MenuBar)
		{
			// File
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem(ICON_FA_FILE_O "  New", "Ctrl+N"))
					NewScene();

				if (ImGui::MenuItem(ICON_FA_FILE  "  Open...", "Ctrl+O"))
					OpenScene();;
				
				if (ImGui::MenuItem(ICON_FA_FLOPPY_O  "  Save", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem(ICON_FA_FLOPPY_O "  Save as...", "Ctrl+Shift+S"))
					SaveSceneAs();

				ImGui::Separator();

				if (ImGui::MenuItem("Exit", "Alt+F4"))
					Application::Get().Close();


				ImGui::EndMenu();
			}

			// Edit
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem(ICON_FA_ARROW_CIRCLE_LEFT "  Undo"))
					CORE_LOG_INFO("Not implemented!");

				if (ImGui::MenuItem(ICON_FA_ARROW_CIRCLE_RIGHT "  Redo"))
					CORE_LOG_INFO("Not implemented!");

				if (ImGui::MenuItem(ICON_FA_MINUS_CIRCLE "  Delete", "Del"))
				{
					if (m_ObjectsPanel.GetSelectedEntity())
					{
						m_ActiveScene->DestroyEntity(m_ObjectsPanel.GetSelectedEntity());
						m_ObjectsPanel.SetSelectedEntity(Entity());
						m_ActiveScene->SetEditorSelectedEntity(Entity());
					}
				}

				ImGui::EndMenu();
			}

			// View
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem(ICON_FA_STAR " Asset Browser", nullptr, m_ShowAssetBrowserPanel))
					m_ShowAssetBrowserPanel = !m_ShowAssetBrowserPanel;

				if (ImGui::MenuItem(ICON_FA_COGS " Scene Properties", nullptr, m_ShowSceneSettingsPanel))
					m_ShowSceneSettingsPanel = !m_ShowSceneSettingsPanel;

				if (ImGui::MenuItem(ICON_FA_SIGNAL " Statistics", nullptr, m_ShowStatsPanel))
					m_ShowStatsPanel = !m_ShowStatsPanel;



				ImGui::EndMenu();
			}

			// Help
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem(ICON_FA_INFO_CIRCLE " About"))
					m_ShowWelcomeMessage = true;

				ImGui::EndMenu();
			}
			
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
				return;
			ImGuiContext& g = *GImGui;

			// Nav: When a move request within one of our child menu failed, capture the request to navigate among our siblings.
			if (ImGui::NavMoveRequestButNoResultYet() && (g.NavMoveDir == ImGuiDir_Left || g.NavMoveDir == ImGuiDir_Right) && (g.NavWindow->Flags & ImGuiWindowFlags_ChildMenu))
			{
				ImGuiWindow* nav_earliest_child = g.NavWindow;
				while (nav_earliest_child->ParentWindow && (nav_earliest_child->ParentWindow->Flags & ImGuiWindowFlags_ChildMenu))
					nav_earliest_child = nav_earliest_child->ParentWindow;
				if (nav_earliest_child->ParentWindow == window && nav_earliest_child->DC.ParentLayoutType == ImGuiLayoutType_Horizontal && g.NavMoveRequestForward == ImGuiNavForward_None)
				{
					// To do so we claim focus back, restore NavId and then process the movement request for yet another frame.
					// This involve a one-frame delay which isn't very problematic in this situation. We could remove it by scoring in advance for multiple window (probably not worth the hassle/cost)
					const ImGuiNavLayer layer = ImGuiNavLayer_Menu;
					IM_ASSERT(window->DC.NavLayersActiveMaskNext & (1 << layer)); // Sanity check
					ImGui::FocusWindow(window);
					ImGui::SetNavID(window->NavLastIds[layer], layer, 0, window->NavRectRel[layer]);
					g.NavDisableHighlight = true; // Hide highlight for the current frame so we don't see the intermediary selection.
					g.NavDisableMouseHover = g.NavMousePosDirty = true;
					g.NavMoveRequestForward = ImGuiNavForward_ForwardQueued;
					ImGui::NavMoveRequestCancel();
				}
			}

			IM_MSVC_WARNING_SUPPRESS(6011); // Static Analysis false positive "warning C6011: Dereferencing NULL pointer 'window'"
			//IM_ASSERT(window->Flags & ImGuiWindowFlags_MenuBar);
			IM_ASSERT(window->DC.MenuBarAppending);
			ImGui::PopClipRect();
			ImGui::PopID();
			window->DC.MenuBarOffset.x = window->DC.CursorPos.x - window->Pos.x; // Save horizontal position so next append can reuse it. This is kinda equivalent to a per-layer CursorPos.
			g.GroupStack.back().EmitItem = false;
			ImGui::EndGroup(); // Restore position on layer 0
			window->DC.LayoutType = ImGuiLayoutType_Vertical;
			window->DC.NavLayerCurrent = ImGuiNavLayer_Main;
			window->DC.MenuBarAppending = false;
		}

		ImGui::EndGroup();
	}

	void EditorLayer::UI_ToolBar()
	{
#if 0
		// Toolbar settings
		auto& colors = ImGui::GetStyle().Colors;

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(colors[ImGuiCol_Button].x, colors[ImGuiCol_Button].y, 
							  colors[ImGuiCol_Button].z, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(colors[ImGuiCol_ButtonHovered].x, colors[ImGuiCol_ButtonHovered].y,
			colors[ImGuiCol_ButtonHovered].z, 0.3f));

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 5));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

		ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

		// Play // Stop
		float size = ImGui::GetWindowHeight() - 10.0f;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_PlayIcon : m_StopIcon;
		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2{ size, size }, ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (m_SceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();
		}
		ImGui::PopStyleColor(2);

		float startPos = 0.13f;
		ImVec4 selectedColor = { 0.85f, 0.8505f, 0.851f, 1.0f };
		ImVec4 unselectedColor = { 0.0f, 0.0f, 0.0f, 0.0f };
		ImVec4 color = { 0.0f, 0.0f, 0.0f, 0.0f };

		// Gizmos Position TODO: Better way to handle button activation
		ImGui::SameLine();

		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * startPos));
		if (m_GizmoType == ImGuizmo::OPERATION::TRANSLATE)
			color = selectedColor;
		else
			color = unselectedColor;
		if (ImGui::ImageButton((ImTextureID)m_GizmosPositionIcon->GetRendererID(), ImVec2{ size, size }, ImVec2(0, 0), ImVec2(1, 1), 0, color))
		{
			m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
		}

		// Gizmos Rotation
		ImGui::SameLine();
		if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
			color = selectedColor;
		else
			color = unselectedColor;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * startPos) + size * 1.5);
		if (ImGui::ImageButton((ImTextureID)m_GizmosRotationIcon->GetRendererID(), ImVec2{ size, size }, ImVec2(0, 0), ImVec2(1, 1), 0, color))
		{
			m_GizmoType = ImGuizmo::OPERATION::ROTATE;
		}

		// Gizmos Scale
		ImGui::SameLine();
		if (m_GizmoType == ImGuizmo::OPERATION::SCALE)
			color = selectedColor;
		else
			color = unselectedColor;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * startPos) + (size * 1.5) * 2);
		if (ImGui::ImageButton((ImTextureID)m_GizmosScaleIcon->GetRendererID(), ImVec2{ size, size }, ImVec2(0, 0), ImVec2(1, 1), 0, color))
		{
			m_GizmoType = ImGuizmo::OPERATION::SCALE;
		}

		// Grid 
		ImGui::SameLine();
		if (m_SceneRenderer->GetOptions().ShowGrid)
			color = selectedColor;
		else 
			color = unselectedColor;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * startPos) + (size * 1.5) * 3);
		if (ImGui::ImageButton((ImTextureID)m_GridIcon->GetRendererID(), ImVec2{ size, size }, ImVec2(0, 0), ImVec2(1, 1), 0, color))
		{
			m_SceneRenderer->GetOptions().ShowGrid = !m_SceneRenderer->GetOptions().ShowGrid;
		}

		// Reset Camera
		ImGui::SameLine();
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * startPos) + (size * 1.5) * 4);
		if (ImGui::ImageButton((ImTextureID)m_EditorCameraIcon->GetRendererID(), ImVec2{ size, size }, ImVec2(0, 0), ImVec2(1, 1), 0, unselectedColor))
		{
			m_EditorCamera = EditorCamera(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
		}

		// Scene Name Temp
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(colors[ImGuiCol_Button].x, colors[ImGuiCol_Button].y,
			colors[ImGuiCol_Button].z, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(colors[ImGuiCol_Button].x, colors[ImGuiCol_Button].y,
			colors[ImGuiCol_Button].z, 1.0f));
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x * 0.9);
		ImGui::SetCursorPosY(0.0f);
		ImGui::Button(m_ActiveScene->m_SceneName.c_str());
		ImGui::PopStyleColor(2);

		ImGui::PopStyleVar(2);
		ImGui::End();
#endif
		auto& colors = ImGui::GetStyle().Colors;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 5));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

		ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

		ImVec4 normalColor = { colors[ImGuiCol_Button].x, colors[ImGuiCol_Button].y, colors[ImGuiCol_Button].z, 0.0f };
		ImGui::PushStyleColor(ImGuiCol_WindowBg, { 1.10f, 0.10f, 0.10f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_Button, normalColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, normalColor);
		ImGui::PushStyleColor(ImGuiCol_Border, normalColor);
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, normalColor);

		ImGui::Columns(5);

		// SAVE BUTTON
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5.0f);

			ImVec4 normalColor = { colors[ImGuiCol_Button].x, colors[ImGuiCol_Button].y, colors[ImGuiCol_Button].z, 0.0f };

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
			if (ImGui::ImageButton((ImTextureID)m_UndoIcon->GetRendererID(), ImVec2{ 20, 20 }, ImVec2(0, 0), ImVec2(1, 1), 0))
			{

			}

			ImGui::SameLine();

			if (ImGui::ImageButton((ImTextureID)m_RedoIcon->GetRendererID(), ImVec2{ 20, 20 }, ImVec2(0, 0), ImVec2(1, 1), 0))
			{

			}

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 30.0f);

			bool clicked = false;
			if (ImGui::ImageButton((ImTextureID)m_SaveIcon->GetRendererID(), ImVec2{ 23, 20 }, ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				clicked = true;
			}

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4.0f);

			ImGui::Button("Save");
			if (ImGui::IsItemClicked())
				clicked = true;

			if (clicked)
			{
				SaveScene();
			}
		}

		// Create
		{
			ImGui::SameLine();
			ImGui::NextColumn();
			ImGui::SetColumnOffset(ImGui::GetColumnIndex(), 190);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

			if (ImGui::ImageButton((ImTextureID)m_CreateIcon->GetRendererID(), ImVec2{ 25, 25 }, ImVec2(0, 0), ImVec2(1, 1), 0))
				ImGui::OpenPopup("CreateToolbar");

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4.0f);

			if(ImGui::Button("Create  " ICON_FA_CARET_DOWN))
				ImGui::OpenPopup("CreateToolbar");

			if (ImGui::BeginPopup("CreateToolbar"))
			{
				if (ImGui::MenuItem("   " ICON_FA_FILE "   Create Empty"))
				{
					auto entity = m_ActiveScene->CreateEntity();
					m_ObjectsPanel.SetSelectedEntity(entity);
					m_ActiveScene->SetEditorSelectedEntity(entity);
				}

				if (ImGui::MenuItem("   " ICON_FA_VIDEO_CAMERA "   Create Camera"))
				{
					auto entity = m_ActiveScene->CreateEntity("Camera");
					entity.AddComponent<CameraComponent>();
					m_ObjectsPanel.SetSelectedEntity(entity);
					m_ActiveScene->SetEditorSelectedEntity(entity);
				}

				ImGui::Separator();

				if (ImGui::BeginMenu("   "  ICON_FA_CUBE "  3D Objects"))
				{
					if (ImGui::MenuItem("   Cube"))
					{
						auto entity = m_ActiveScene->CreateEntity("Cube");
						entity.AddComponent<MeshRendererComponent>(); // Default is cube
						m_ObjectsPanel.SetSelectedEntity(entity);
						m_ActiveScene->SetEditorSelectedEntity(entity);
					}

					if (ImGui::MenuItem("   Sphere"))
					{
						auto entity = m_ActiveScene->CreateEntity("Sphere");
						auto& component = entity.AddComponent<MeshRendererComponent>();

						component.Model = Model::Create("Resources/Models/Sphere.fbx");
						component.ModelName = "Sphere";

						m_ObjectsPanel.SetSelectedEntity(entity);
						m_ActiveScene->SetEditorSelectedEntity(entity);
					}

					if (ImGui::MenuItem("   Plane"))
					{
						auto entity = m_ActiveScene->CreateEntity("Plane");
						auto& component = entity.AddComponent<MeshRendererComponent>();

						component.Model = Model::Create("Resources/Models/Plane.fbx");
						component.ModelName = "Plane";

						m_ObjectsPanel.SetSelectedEntity(entity);
						m_ActiveScene->SetEditorSelectedEntity(entity);
					}

					if (ImGui::MenuItem("   Capsule"))
					{
						auto entity = m_ActiveScene->CreateEntity("Capsule");
						auto& component = entity.AddComponent<MeshRendererComponent>();

						component.Model = Model::Create("Resources/Models/Capsule.fbx");
						component.ModelName = "Capsule";

						m_ObjectsPanel.SetSelectedEntity(entity);
						m_ActiveScene->SetEditorSelectedEntity(entity);
					}

					if (ImGui::MenuItem("   Torus"))
					{
						auto entity = m_ActiveScene->CreateEntity("Torus");
						auto& component = entity.AddComponent<MeshRendererComponent>();

						component.Model = Model::Create("Resources/Models/Torus.fbx");
						component.ModelName = "Torus";

						m_ObjectsPanel.SetSelectedEntity(entity);
						m_ActiveScene->SetEditorSelectedEntity(entity);
					}

					if (ImGui::MenuItem("   Cone"))
					{
						auto entity = m_ActiveScene->CreateEntity("Cone");
						auto& component = entity.AddComponent<MeshRendererComponent>();

						component.Model = Model::Create("Resources/Models/Cone.fbx");
						component.ModelName = "Cone";

						m_ObjectsPanel.SetSelectedEntity(entity);
						m_ActiveScene->SetEditorSelectedEntity(entity);
					}

					if (ImGui::MenuItem("   Cylinder"))
					{
						auto entity = m_ActiveScene->CreateEntity("Cylinder");
						auto& component = entity.AddComponent<MeshRendererComponent>();

						component.Model = Model::Create("Resources/Models/Cylinder.fbx");
						component.ModelName = "Cylinder";

						m_ObjectsPanel.SetSelectedEntity(entity);
						m_ActiveScene->SetEditorSelectedEntity(entity);
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("   " ICON_FA_FILE_IMAGE_O "  2D Objects"))
				{
					if (ImGui::MenuItem("   Sprite"))
					{
						auto entity = m_ActiveScene->CreateEntity("Sprite");
						entity.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
						m_ObjectsPanel.SetSelectedEntity(entity);
						m_ActiveScene->SetEditorSelectedEntity(entity);
					}

					if (ImGui::MenuItem("   Circle"))
					{
						auto entity = m_ActiveScene->CreateEntity("Circle");
						entity.AddComponent<CircleRendererComponent>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
						m_ObjectsPanel.SetSelectedEntity(entity);
						m_ActiveScene->SetEditorSelectedEntity(entity);
					}

					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::BeginMenu("   " ICON_FA_LIGHTBULB_O "   Light"))
				{
					if (ImGui::MenuItem("   Sky Light"))
					{
						auto entity = m_ActiveScene->CreateEntity("Sky Light");
						entity.AddComponent<SkyLightComponent>();
						m_ObjectsPanel.SetSelectedEntity(entity);
						m_ActiveScene->SetEditorSelectedEntity(entity);
					}

					if (ImGui::MenuItem("   Directional Light"))
					{
						auto entity = m_ActiveScene->CreateEntity("Directional Light");
						auto& transform = entity.GetComponent<TransformComponent>();
						transform.Rotation = glm::radians(glm::vec3({ 80.0f, 10.0f, 0.0f }));
						entity.AddComponent<DirectionalLightComponent>();
						m_ObjectsPanel.SetSelectedEntity(entity);
						m_ActiveScene->SetEditorSelectedEntity(entity);
					}

					if (ImGui::MenuItem("   Point Light"))
					{
						auto entity = m_ActiveScene->CreateEntity("Point Light");
						entity.AddComponent<PointLightComponent>();
						m_ObjectsPanel.SetSelectedEntity(entity);
						m_ActiveScene->SetEditorSelectedEntity(entity);
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}
		}

		// PLAY-STOP BUTTON
		{
			ImGui::SameLine();
			ImGui::NextColumn();
			ImGui::SetColumnOffset(ImGui::GetColumnIndex(), 340);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

			bool clicked = false;

			Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_PlayIcon : m_StopIcon;
			std::string text = m_SceneState == SceneState::Edit ? "Play" : "Stop";
			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2{ 20, 20 }, ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				clicked = true;
			}

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4.0f);

			ImGui::Button(text.c_str());
			if (ImGui::IsItemClicked())
				clicked = true;

			if (clicked)
			{
				if (m_SceneState == SceneState::Edit)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}
		}
		
		// Gizmos
		{
			ImGui::SameLine();
			ImGui::NextColumn();
			ImGui::SetColumnOffset(ImGui::GetColumnIndex(), 450);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, { 1.0, 1.0, 1.0, 0.5 });
			ImGui::Button("Selection");
			ImGui::PopStyleColor();

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8.0f);

			std::string gizmosType[] = { ICON_FA_MOUSE_POINTER "   None", ICON_FA_ARROWS "   Translate",
										 ICON_FA_HISTORY "   Rotate", ICON_FA_ANGLE_DOUBLE_UP "   Scale" };
			std::string currentGyzmosType = gizmosType[m_GizmoType + 1];

			if (ImGui::BeginCombo("##GizmosType", currentGyzmosType.c_str(), ImGuiComboFlags_NoArrowButton))
			{
				for (int i = 0; i < 4; i++)
				{
					bool isSelected = currentGyzmosType == gizmosType[i];
					if (ImGui::Selectable(gizmosType[i].c_str(), isSelected))
					{
						currentGyzmosType = gizmosType[i];
						m_GizmoType = i - 1;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

		}

		// Camera
		{
			ImGui::SameLine();
			ImGui::NextColumn();
			ImGui::SetColumnOffset(ImGui::GetColumnIndex(), 840);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
		
			if (ImGui::ImageButton((ImTextureID)m_EditorCameraIcon->GetRendererID(), ImVec2{ 28, 28 }, ImVec2(0, 0), ImVec2(1, 1), 0))
				ImGui::OpenPopup("CameraOptionsPopup");

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4.0f);

			if (ImGui::Button("Editor Camera  " ICON_FA_CARET_DOWN))
				ImGui::OpenPopup("CameraOptionsPopup");
		

			if (ImGui::BeginPopup("CameraOptionsPopup"))
			{
				if (ImGui::MenuItem("   Reset Camera"))
				{
					m_EditorCamera = EditorCamera(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
				}

				if (ImGui::MenuItem("   Focus on Selected", "F"))
				{
					if (m_ObjectsPanel.GetSelectedEntity())
						m_EditorCamera.Focus(m_ObjectsPanel.GetSelectedEntity().GetComponent<TransformComponent>().Position);
				}

				ImGui::EndPopup();
			}
		}

		// Settings
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - 120.0f);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);

			SceneRendererOptions& options = m_SceneRenderer->GetOptions();

			if (ImGui::ImageButton((ImTextureID)m_SettingsIcon->GetRendererID(), ImVec2{ 20, 20 }, ImVec2(0, 0), ImVec2(1, 1), 0))
				ImGui::OpenPopup("SettingsToolbarPopup");

			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4.0f);

			if (ImGui::Button("Settings  " ICON_FA_CARET_DOWN))
				ImGui::OpenPopup("SettingsToolbarPopup");

			if (ImGui::BeginPopup("SettingsToolbarPopup"))
			{
				ImGui::Text("   Main Viewport");
				ImGui::Separator();
				ImGui::Checkbox("  Show Grid", &options.ShowGrid);
				ImGui::Checkbox("  Enable Bloom", &options.Bloom);
				ImGui::Checkbox("  Enable FXAA", &options.FXAA);
				ImGui::Checkbox("  Grayscale", &options.Grayscale);
				ImGui::DragFloat("Exposure", &options.Exposure, 0.1f, 0.0f, 1000.0f);
				ImGui::Checkbox("  ACES Tone Map", &options.ACESTone);
				ImGui::Checkbox("  Gamma Correction", &options.GammaCorrection);

				ImGui::EndPopup();
			}
		}

		ImGui::Columns(1);
		ImGui::PopStyleColor(5);

		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	void EditorLayer::UI_Viewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		//Application::Get().GetImGuiLayer()->BlockEvents(m_ViewportFocused && m_ViewportHovered);

		// Set Viewport Size
		glm::vec2 viewportPanelSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
		m_ViewportSize = viewportPanelSize;

		m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneRenderer->SetViewportSize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

		// Viewport Image
		uint32_t textureID = m_SceneRenderer->GetFinalImage();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		// Viewport Drag and Drop
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_SCENE"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(std::filesystem::path(g_AssetPath) / path);
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::string name = std::filesystem::path(path).stem().string();
				Entity entity = m_ActiveScene->CreateEntity(name);
				auto& src = entity.AddComponent<SpriteRendererComponent>();

				std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
				src.Texture = Texture2D::Create(texturePath.string());
				src.TextureName = texturePath.stem().string();
				src.TexturePath = texturePath.string();

				m_ObjectsPanel.SetSelectedEntity(entity);
			}

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_MODEL"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				std::string name = std::filesystem::path(path).stem().string();
				Entity entity = m_ActiveScene->CreateEntity(name);
				auto& meshRenderer = entity.AddComponent<MeshRendererComponent>();

				std::filesystem::path modelPath = std::filesystem::path(g_AssetPath) / path;
				meshRenderer.Model = Model::Create(modelPath.string());
				meshRenderer.ModelName = modelPath.stem().string();
				meshRenderer.ModelPath = modelPath.string();

				m_ObjectsPanel.SetSelectedEntity(entity);
			}

		}

		// Gizmos TEMP
		Entity selectedEntity = m_ObjectsPanel.GetSelectedEntity();
		if (m_SceneState == SceneState::Edit && selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// Editor Camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjectionMatrix();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			// Entity
			auto& transformCompenent = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = transformCompenent.GetTransform();

			// Snapping 
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f;
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 position, rotation, scale;
				Math::DecomposeTransform(transform, position, rotation, scale);

				glm::vec3 deltaRotation = rotation - transformCompenent.Rotation;

				transformCompenent.Position = position;
				transformCompenent.Rotation += deltaRotation;
				transformCompenent.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::UI_Settings()
	{
	}

	void EditorLayer::UI_WindowBorder()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		struct ImGuiResizeBorderDef
		{
			ImVec2 InnerDir;
			ImVec2 SegmentN1, SegmentN2;
			float  OuterAngle;
		};

		static const ImGuiResizeBorderDef resize_border_def[4] =
		{
			{ ImVec2(+1, 0), ImVec2(0, 1), ImVec2(0, 0), IM_PI * 1.00f }, // Left
			{ ImVec2(-1, 0), ImVec2(1, 0), ImVec2(1, 1), IM_PI * 0.00f }, // Right
			{ ImVec2(0, +1), ImVec2(0, 0), ImVec2(1, 0), IM_PI * 1.50f }, // Up
			{ ImVec2(0, -1), ImVec2(1, 1), ImVec2(0, 1), IM_PI * 0.50f }  // Down
		};

		auto GetResizeBorderRect = [](ImGuiWindow* window, int border_n, float perp_padding, float thickness)
		{
			ImRect rect = window->Rect();
			if (thickness == 0.0f)
			{
				rect.Max.x -= 1;
				rect.Max.y -= 1;
			}
			if (border_n == ImGuiDir_Left) { return ImRect(rect.Min.x - thickness, rect.Min.y + perp_padding, rect.Min.x + thickness, rect.Max.y - perp_padding); }
			if (border_n == ImGuiDir_Right) { return ImRect(rect.Max.x - thickness, rect.Min.y + perp_padding, rect.Max.x + thickness, rect.Max.y - perp_padding); }
			if (border_n == ImGuiDir_Up) { return ImRect(rect.Min.x + perp_padding, rect.Min.y - thickness, rect.Max.x - perp_padding, rect.Min.y + thickness); }
			if (border_n == ImGuiDir_Down) { return ImRect(rect.Min.x + perp_padding, rect.Max.y - thickness, rect.Max.x - perp_padding, rect.Max.y + thickness); }
			IM_ASSERT(0);
			return ImRect();
		};


		ImGuiContext& g = *GImGui;
		float rounding = window->WindowRounding;
		float border_size = 1.0f; // window->WindowBorderSize;
		if (border_size > 0.0f && !(window->Flags & ImGuiWindowFlags_NoBackground))
			window->DrawList->AddRect(window->Pos, { window->Pos.x + window->Size.x,  window->Pos.y + window->Size.y }, ImGui::GetColorU32(ImGuiCol_Border), rounding, 0, border_size);

		int border_held = window->ResizeBorderHeld;
		if (border_held != -1)
		{
			const ImGuiResizeBorderDef& def = resize_border_def[border_held];
			ImRect border_r = GetResizeBorderRect(window, border_held, rounding, 0.0f);
			ImVec2 p1 = ImLerp(border_r.Min, border_r.Max, def.SegmentN1);
			const float offsetX = def.InnerDir.x * rounding;
			const float offsetY = def.InnerDir.y * rounding;
			p1.x += 0.5f + offsetX;
			p1.y += 0.5f + offsetY;

			ImVec2 p2 = ImLerp(border_r.Min, border_r.Max, def.SegmentN2);
			p2.x += 0.5f + offsetX;
			p2.y += 0.5f + offsetY;

			window->DrawList->PathArcTo(p1, rounding, def.OuterAngle - IM_PI * 0.25f, def.OuterAngle);
			window->DrawList->PathArcTo(p2, rounding, def.OuterAngle, def.OuterAngle + IM_PI * 0.25f);
			window->DrawList->PathStroke(ImGui::GetColorU32(ImGuiCol_SeparatorActive), 0, ImMax(2.0f, border_size)); // Thicker than usual
		}
		if (g.Style.FrameBorderSize > 0 && !(window->Flags & ImGuiWindowFlags_NoTitleBar) && !window->DockIsActive)
		{
			float y = window->Pos.y + window->TitleBarHeight() - 1;
			window->DrawList->AddLine(ImVec2(window->Pos.x + border_size, y), ImVec2(window->Pos.x + window->Size.x - border_size, y), ImGui::GetColorU32(ImGuiCol_Border), g.Style.FrameBorderSize);
		}
	}

	void EditorLayer::OnEvent(Event& e)
	{
		if(m_SceneState == SceneState::Edit && m_ViewportHovered)
			m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(VS_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(VS_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
		dispatcher.Dispatch<WindowTitleBarHitTestEvent>(VS_BIND_EVENT_FN(EditorLayer::OnTitleBarHit));
	}

	void EditorLayer::NewScene()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

		m_ObjectsPanel.SetContext(m_EditorScene);
		m_AssetBrowserPanel.SetContext(m_EditorScene);

		UpdateWindowTitle(m_EditorScene->m_SceneName);

		m_ScenePath = std::string();

		m_ActiveScene = m_EditorScene;
		
		m_SceneRenderer->SetScene(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filePath = FileDialogs::OpenFile("Venus Scene (*.venus)\0*.venus\0");

		if (!filePath.empty())
		{
			OpenScene(filePath);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_EditorScene = CreateRef<Scene>();
		m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

		SceneSerializer serializer(m_EditorScene);
		serializer.Deserialize(path.string());

		m_ScenePath = path.string();

		UpdateWindowTitle(m_EditorScene->m_SceneName);

		m_ObjectsPanel.SetContext(m_EditorScene);
		m_AssetBrowserPanel.SetContext(m_EditorScene);

		m_ActiveScene = m_EditorScene;

		m_SceneRenderer->SetScene(m_ActiveScene);
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filePath = FileDialogs::SaveFile("Venus Scene (*.venus)\0*.venus\0");

		if (!filePath.empty())
		{
			int nameStart = filePath.find_last_of("\\") + 1;
			m_EditorScene->m_SceneName = filePath.substr(nameStart, filePath.size() - nameStart - 6);

			SceneSerializer serializer(m_EditorScene);
			serializer.Serialize(filePath);

			UpdateWindowTitle(m_EditorScene->m_SceneName);

			m_ScenePath = filePath;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_ScenePath.empty())
		{
			SceneSerializer serializer(m_EditorScene);
			serializer.Serialize(m_ScenePath);
		}
		else
			SaveSceneAs();
	}

	void EditorLayer::OnScenePlay()
	{
		m_RuntimeScene = Scene::Copy(m_EditorScene);
		m_RuntimeScene->OnRuntimeStart();
		m_ObjectsPanel.SetContext(m_RuntimeScene);
		m_AssetBrowserPanel.SetContext(m_RuntimeScene);

		m_ActiveScene = m_RuntimeScene;

		m_SceneState = SceneState::Play;

		m_SceneRenderer->SetScene(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		m_RuntimeScene->OnRuntimeStop();
		m_RuntimeScene = nullptr; 
		m_ObjectsPanel.SetContext(m_EditorScene);
		m_AssetBrowserPanel.SetContext(m_EditorScene);

		m_ActiveScene = m_EditorScene;

		m_SceneState = SceneState::Edit;

		m_SceneRenderer->SetScene(m_ActiveScene);
	}

	void EditorLayer::UpdateWindowTitle(const std::string& sceneName)
	{
		std::string rendererApi = RendererAPI::GetAPIName();
		Application::Get().GetWindow().SetWindowTitle(sceneName + ".venus - Venus Editor - " + rendererApi);
	}

	void EditorLayer::UpdateHoveredEntity()
	{
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_SceneRenderer->GetGeometryBuffer()->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}
	}

	bool EditorLayer::OnManualWindowResize()
	{
		ImVec2 newSize, newPosition;
		auto window = ImGui::GetCurrentWindow();

		auto CalcWindowSizeAfterConstraint = [](ImGuiWindow* window, const ImVec2& size_desired)
		{
			ImGuiContext& g = *GImGui;
			ImVec2 new_size = size_desired;
			if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint)
			{
				// Using -1,-1 on either X/Y axis to preserve the current size.
				ImRect cr = g.NextWindowData.SizeConstraintRect;
				new_size.x = (cr.Min.x >= 0 && cr.Max.x >= 0) ? ImClamp(new_size.x, cr.Min.x, cr.Max.x) : window->SizeFull.x;
				new_size.y = (cr.Min.y >= 0 && cr.Max.y >= 0) ? ImClamp(new_size.y, cr.Min.y, cr.Max.y) : window->SizeFull.y;
				if (g.NextWindowData.SizeCallback)
				{
					ImGuiSizeCallbackData data;
					data.UserData = g.NextWindowData.SizeCallbackUserData;
					data.Pos = window->Pos;
					data.CurrentSize = window->SizeFull;
					data.DesiredSize = new_size;
					g.NextWindowData.SizeCallback(&data);
					new_size = data.DesiredSize;
				}
				new_size.x = IM_FLOOR(new_size.x);
				new_size.y = IM_FLOOR(new_size.y);
			}

			// Minimum size
			if (!(window->Flags & (ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_AlwaysAutoResize)))
			{
				ImGuiWindow* window_for_height = (window->DockNodeAsHost && window->DockNodeAsHost->VisibleWindow) ? window->DockNodeAsHost->VisibleWindow : window;
				const float decoration_up_height = window_for_height->TitleBarHeight() + window_for_height->MenuBarHeight();
				new_size = ImMax(new_size, g.Style.WindowMinSize);
				new_size.y = ImMax(new_size.y, decoration_up_height + ImMax(0.0f, g.Style.WindowRounding - 1.0f)); // Reduce artifacts with very small windows
			}
			return new_size;
		};

		auto CalcWindowAutoFitSize = [CalcWindowSizeAfterConstraint](ImGuiWindow* window, const ImVec2& size_contents)
		{
			ImGuiContext& g = *GImGui;
			ImGuiStyle& style = g.Style;
			const float decoration_up_height = window->TitleBarHeight() + window->MenuBarHeight();
			ImVec2 size_pad{ window->WindowPadding.x * 2.0f, window->WindowPadding.y * 2.0f };
			ImVec2 size_desired = { size_contents.x + size_pad.x + 0.0f, size_contents.y + size_pad.y + decoration_up_height };
			if (window->Flags & ImGuiWindowFlags_Tooltip)
			{
				// Tooltip always resize
				return size_desired;
			}
			else
			{
				// Maximum window size is determined by the viewport size or monitor size
				const bool is_popup = (window->Flags & ImGuiWindowFlags_Popup) != 0;
				const bool is_menu = (window->Flags & ImGuiWindowFlags_ChildMenu) != 0;
				ImVec2 size_min = style.WindowMinSize;
				if (is_popup || is_menu) // Popups and menus bypass style.WindowMinSize by default, but we give then a non-zero minimum size to facilitate understanding problematic cases (e.g. empty popups)
					size_min = ImMin(size_min, ImVec2(4.0f, 4.0f));

				// FIXME-VIEWPORT-WORKAREA: May want to use GetWorkSize() instead of Size depending on the type of windows?
				ImVec2 avail_size = window->Viewport->Size;
				if (window->ViewportOwned)
					avail_size = ImVec2(FLT_MAX, FLT_MAX);
				const int monitor_idx = window->ViewportAllowPlatformMonitorExtend;
				if (monitor_idx >= 0 && monitor_idx < g.PlatformIO.Monitors.Size)
					avail_size = g.PlatformIO.Monitors[monitor_idx].WorkSize;
				ImVec2 size_auto_fit = ImClamp(size_desired, size_min, ImMax(size_min, { avail_size.x - style.DisplaySafeAreaPadding.x * 2.0f,
																						avail_size.y - style.DisplaySafeAreaPadding.y * 2.0f }));

				// When the window cannot fit all contents (either because of constraints, either because screen is too small),
				// we are growing the size on the other axis to compensate for expected scrollbar. FIXME: Might turn bigger than ViewportSize-WindowPadding.
				ImVec2 size_auto_fit_after_constraint = CalcWindowSizeAfterConstraint(window, size_auto_fit);
				bool will_have_scrollbar_x = (size_auto_fit_after_constraint.x - size_pad.x - 0.0f < size_contents.x && !(window->Flags & ImGuiWindowFlags_NoScrollbar) && (window->Flags & ImGuiWindowFlags_HorizontalScrollbar)) || (window->Flags & ImGuiWindowFlags_AlwaysHorizontalScrollbar);
				bool will_have_scrollbar_y = (size_auto_fit_after_constraint.y - size_pad.y - decoration_up_height < size_contents.y && !(window->Flags& ImGuiWindowFlags_NoScrollbar)) || (window->Flags & ImGuiWindowFlags_AlwaysVerticalScrollbar);
				if (will_have_scrollbar_x)
					size_auto_fit.y += style.ScrollbarSize;
				if (will_have_scrollbar_y)
					size_auto_fit.x += style.ScrollbarSize;
				return size_auto_fit;
			}
		};

		ImGuiContext& g = *GImGui;

		// Decide if we are going to handle borders and resize grips
		const bool handle_borders_and_resize_grips = (window->DockNodeAsHost || !window->DockIsActive);

		if (!handle_borders_and_resize_grips || window->Collapsed)
			return false;

		const ImVec2 size_auto_fit = CalcWindowAutoFitSize(window, window->ContentSizeIdeal);

		// Handle manual resize: Resize Grips, Borders, Gamepad
		int border_held = -1;
		ImU32 resize_grip_col[4] = {};
		const int resize_grip_count = g.IO.ConfigWindowsResizeFromEdges ? 2 : 1; // Allow resize from lower-left if we have the mouse cursor feedback for it.
		const float resize_grip_draw_size = IM_FLOOR(ImMax(g.FontSize * 1.10f, window->WindowRounding + 1.0f + g.FontSize * 0.2f));
		window->ResizeBorderHeld = (signed char)border_held;

		//const ImRect& visibility_rect;

		struct ImGuiResizeBorderDef
		{
			ImVec2 InnerDir;
			ImVec2 SegmentN1, SegmentN2;
			float  OuterAngle;
		};
		static const ImGuiResizeBorderDef resize_border_def[4] =
		{
			{ ImVec2(+1, 0), ImVec2(0, 1), ImVec2(0, 0), IM_PI * 1.00f }, // Left
			{ ImVec2(-1, 0), ImVec2(1, 0), ImVec2(1, 1), IM_PI * 0.00f }, // Right
			{ ImVec2(0, +1), ImVec2(0, 0), ImVec2(1, 0), IM_PI * 1.50f }, // Up
			{ ImVec2(0, -1), ImVec2(1, 1), ImVec2(0, 1), IM_PI * 0.50f }  // Down
		};

		// Data for resizing from corner
		struct ImGuiResizeGripDef
		{
			ImVec2  CornerPosN;
			ImVec2  InnerDir;
			int     AngleMin12, AngleMax12;
		};
		static const ImGuiResizeGripDef resize_grip_def[4] =
		{
			{ ImVec2(1, 1), ImVec2(-1, -1), 0, 3 },  // Lower-right
			{ ImVec2(0, 1), ImVec2(+1, -1), 3, 6 },  // Lower-left
			{ ImVec2(0, 0), ImVec2(+1, +1), 6, 9 },  // Upper-left (Unused)
			{ ImVec2(1, 0), ImVec2(-1, +1), 9, 12 }  // Upper-right (Unused)
		};

		auto CalcResizePosSizeFromAnyCorner = [CalcWindowSizeAfterConstraint](ImGuiWindow* window, const ImVec2& corner_target, const ImVec2& corner_norm, ImVec2* out_pos, ImVec2* out_size)
		{
			ImVec2 pos_min = ImLerp(corner_target, window->Pos, corner_norm);                // Expected window upper-left
			ImVec2 pos_max = ImLerp({ window->Pos.x + window->Size.x, window->Pos.y + window->Size.y }, corner_target, corner_norm); // Expected window lower-right
			ImVec2 size_expected = { pos_max.x - pos_min.x,  pos_max.y - pos_min.y };
			ImVec2 size_constrained = CalcWindowSizeAfterConstraint(window, size_expected);
			*out_pos = pos_min;
			if (corner_norm.x == 0.0f)
				out_pos->x -= (size_constrained.x - size_expected.x);
			if (corner_norm.y == 0.0f)
				out_pos->y -= (size_constrained.y - size_expected.y);
			*out_size = size_constrained;
		};

		auto GetResizeBorderRect = [](ImGuiWindow* window, int border_n, float perp_padding, float thickness)
		{
			ImRect rect = window->Rect();
			if (thickness == 0.0f)
			{
				rect.Max.x -= 1;
				rect.Max.y -= 1;
			}
			if (border_n == ImGuiDir_Left) { return ImRect(rect.Min.x - thickness, rect.Min.y + perp_padding, rect.Min.x + thickness, rect.Max.y - perp_padding); }
			if (border_n == ImGuiDir_Right) { return ImRect(rect.Max.x - thickness, rect.Min.y + perp_padding, rect.Max.x + thickness, rect.Max.y - perp_padding); }
			if (border_n == ImGuiDir_Up) { return ImRect(rect.Min.x + perp_padding, rect.Min.y - thickness, rect.Max.x - perp_padding, rect.Min.y + thickness); }
			if (border_n == ImGuiDir_Down) { return ImRect(rect.Min.x + perp_padding, rect.Max.y - thickness, rect.Max.x - perp_padding, rect.Max.y + thickness); }
			IM_ASSERT(0);
			return ImRect();
		};

		static const float WINDOWS_HOVER_PADDING = 4.0f;                        // Extend outside window for hovering/resizing (maxxed with TouchPadding) and inside windows for borders. Affect FindHoveredWindow().
		static const float WINDOWS_RESIZE_FROM_EDGES_FEEDBACK_TIMER = 0.04f;    // Reduce visual noise by only highlighting the border after a certain time.

		auto& style = g.Style;
		ImGuiWindowFlags flags = window->Flags;

		if (/*(flags & ImGuiWindowFlags_NoResize) || */(flags & ImGuiWindowFlags_AlwaysAutoResize) || window->AutoFitFramesX > 0 || window->AutoFitFramesY > 0)
			return false;
		if (window->WasActive == false) // Early out to avoid running this code for e.g. an hidden implicit/fallback Debug window.
			return false;

		bool ret_auto_fit = false;
		const int resize_border_count = g.IO.ConfigWindowsResizeFromEdges ? 4 : 0;
		const float grip_draw_size = IM_FLOOR(ImMax(g.FontSize * 1.35f, window->WindowRounding + 1.0f + g.FontSize * 0.2f));
		const float grip_hover_inner_size = IM_FLOOR(grip_draw_size * 0.75f);
		const float grip_hover_outer_size = g.IO.ConfigWindowsResizeFromEdges ? WINDOWS_HOVER_PADDING : 0.0f;

		ImVec2 pos_target(FLT_MAX, FLT_MAX);
		ImVec2 size_target(FLT_MAX, FLT_MAX);

		// Calculate the range of allowed position for that window (to be movable and visible past safe area padding)
		// When clamping to stay visible, we will enforce that window->Pos stays inside of visibility_rect.
		ImRect viewport_rect(window->Viewport->GetMainRect());
		ImRect viewport_work_rect(window->Viewport->GetWorkRect());
		ImVec2 visibility_padding = ImMax(style.DisplayWindowPadding, style.DisplaySafeAreaPadding);
		ImRect visibility_rect({ viewport_work_rect.Min.x + visibility_padding.x, viewport_work_rect.Min.y + visibility_padding.y },
			{ viewport_work_rect.Max.x - visibility_padding.x, viewport_work_rect.Max.y - visibility_padding.y });

		// Clip mouse interaction rectangles within the viewport rectangle (in practice the narrowing is going to happen most of the time).
		// - Not narrowing would mostly benefit the situation where OS windows _without_ decoration have a threshold for hovering when outside their limits.
		//   This is however not the case with current backends under Win32, but a custom borderless window implementation would benefit from it.
		// - When decoration are enabled we typically benefit from that distance, but then our resize elements would be conflicting with OS resize elements, so we also narrow.
		// - Note that we are unable to tell if the platform setup allows hovering with a distance threshold (on Win32, decorated window have such threshold).
		// We only clip interaction so we overwrite window->ClipRect, cannot call PushClipRect() yet as DrawList is not yet setup.
		const bool clip_with_viewport_rect = !(g.IO.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport) || (g.IO.MouseHoveredViewport != window->ViewportId) || !(window->Viewport->Flags & ImGuiViewportFlags_NoDecoration);
		if (clip_with_viewport_rect)
			window->ClipRect = window->Viewport->GetMainRect();

		// Resize grips and borders are on layer 1
		window->DC.NavLayerCurrent = ImGuiNavLayer_Menu;

		// Manual resize grips
		ImGui::PushID("#RESIZE");
		for (int resize_grip_n = 0; resize_grip_n < resize_grip_count; resize_grip_n++)
		{
			const ImGuiResizeGripDef& def = resize_grip_def[resize_grip_n];

			const ImVec2 corner = ImLerp(window->Pos, { window->Pos.x + window->Size.x, window->Pos.y + window->Size.y }, def.CornerPosN);

			// Using the FlattenChilds button flag we make the resize button accessible even if we are hovering over a child window
			bool hovered, held;
			const ImVec2 min = { corner.x - def.InnerDir.x * grip_hover_outer_size, corner.y - def.InnerDir.y * grip_hover_outer_size };
			const ImVec2 max = { corner.x + def.InnerDir.x * grip_hover_outer_size, corner.y + def.InnerDir.y * grip_hover_outer_size };
			ImRect resize_rect(min, max);

			if (resize_rect.Min.x > resize_rect.Max.x) ImSwap(resize_rect.Min.x, resize_rect.Max.x);
			if (resize_rect.Min.y > resize_rect.Max.y) ImSwap(resize_rect.Min.y, resize_rect.Max.y);
			ImGuiID resize_grip_id = window->GetID(resize_grip_n); // == GetWindowResizeCornerID()
			ImGui::ButtonBehavior(resize_rect, resize_grip_id, &hovered, &held, ImGuiButtonFlags_FlattenChildren | ImGuiButtonFlags_NoNavFocus);
			//GetForegroundDrawList(window)->AddRect(resize_rect.Min, resize_rect.Max, IM_COL32(255, 255, 0, 255));
			if (hovered || held)
				g.MouseCursor = (resize_grip_n & 1) ? ImGuiMouseCursor_ResizeNESW : ImGuiMouseCursor_ResizeNWSE;

			if (held && g.IO.MouseDoubleClicked[0] && resize_grip_n == 0)
			{
				// Manual auto-fit when double-clicking
				size_target = CalcWindowSizeAfterConstraint(window, size_auto_fit);
				ret_auto_fit = true;
				ImGui::ClearActiveID();
			}
			else if (held)
			{
				// Resize from any of the four corners
				// We don't use an incremental MouseDelta but rather compute an absolute target size based on mouse position
				ImVec2 clamp_min = ImVec2(def.CornerPosN.x == 1.0f ? visibility_rect.Min.x : -FLT_MAX, def.CornerPosN.y == 1.0f ? visibility_rect.Min.y : -FLT_MAX);
				ImVec2 clamp_max = ImVec2(def.CornerPosN.x == 0.0f ? visibility_rect.Max.x : +FLT_MAX, def.CornerPosN.y == 0.0f ? visibility_rect.Max.y : +FLT_MAX);

				const float x = g.IO.MousePos.x - g.ActiveIdClickOffset.x + ImLerp(def.InnerDir.x * grip_hover_outer_size, def.InnerDir.x * -grip_hover_inner_size, def.CornerPosN.x);
				const float y = g.IO.MousePos.y - g.ActiveIdClickOffset.y + ImLerp(def.InnerDir.y * grip_hover_outer_size, def.InnerDir.y * -grip_hover_inner_size, def.CornerPosN.y);

				ImVec2 corner_target(x, y); // Corner of the window corresponding to our corner grip
				corner_target = ImClamp(corner_target, clamp_min, clamp_max);
				CalcResizePosSizeFromAnyCorner(window, corner_target, def.CornerPosN, &pos_target, &size_target);
			}

			// Only lower-left grip is visible before hovering/activating
			if (resize_grip_n == 0 || held || hovered)
				resize_grip_col[resize_grip_n] = ImGui::GetColorU32(held ? ImGuiCol_ResizeGripActive : hovered ? ImGuiCol_ResizeGripHovered : ImGuiCol_ResizeGrip);
		}
		for (int border_n = 0; border_n < resize_border_count; border_n++)
		{
			const ImGuiResizeBorderDef& def = resize_border_def[border_n];
			const ImGuiAxis axis = (border_n == ImGuiDir_Left || border_n == ImGuiDir_Right) ? ImGuiAxis_X : ImGuiAxis_Y;

			bool hovered, held;
			ImRect border_rect = GetResizeBorderRect(window, border_n, grip_hover_inner_size, WINDOWS_HOVER_PADDING);
			ImGuiID border_id = window->GetID(border_n + 4); // == GetWindowResizeBorderID()
			ImGui::ButtonBehavior(border_rect, border_id, &hovered, &held, ImGuiButtonFlags_FlattenChildren);
			//GetForegroundDrawLists(window)->AddRect(border_rect.Min, border_rect.Max, IM_COL32(255, 255, 0, 255));
			if ((hovered && g.HoveredIdTimer > WINDOWS_RESIZE_FROM_EDGES_FEEDBACK_TIMER) || held)
			{
				g.MouseCursor = (axis == ImGuiAxis_X) ? ImGuiMouseCursor_ResizeEW : ImGuiMouseCursor_ResizeNS;
				if (held)
					border_held = border_n;
			}
			if (held)
			{
				ImVec2 clamp_min(border_n == ImGuiDir_Right ? visibility_rect.Min.x : -FLT_MAX, border_n == ImGuiDir_Down ? visibility_rect.Min.y : -FLT_MAX);
				ImVec2 clamp_max(border_n == ImGuiDir_Left ? visibility_rect.Max.x : +FLT_MAX, border_n == ImGuiDir_Up ? visibility_rect.Max.y : +FLT_MAX);
				ImVec2 border_target = window->Pos;
				border_target[axis] = g.IO.MousePos[axis] - g.ActiveIdClickOffset[axis] + WINDOWS_HOVER_PADDING;
				border_target = ImClamp(border_target, clamp_min, clamp_max);
				CalcResizePosSizeFromAnyCorner(window, border_target, ImMin(def.SegmentN1, def.SegmentN2), &pos_target, &size_target);
			}
		}
		ImGui::PopID();

		bool changed = false;
		newSize = window->Size;
		newPosition = window->Pos;

		// Apply back modified position/size to window
		if (size_target.x != FLT_MAX)
		{
			//window->SizeFull = size_target;
			//MarkIniSettingsDirty(window);
			newSize = size_target;
			changed = true;
		}
		if (pos_target.x != FLT_MAX)
		{
			//window->Pos = ImFloor(pos_target);
			//MarkIniSettingsDirty(window);
			newPosition = pos_target;
			changed = true;
		}

		//window->Size = window->SizeFull;

		auto* glfwWindow = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		const bool maximized = (bool)glfwGetWindowAttrib(glfwWindow, GLFW_MAXIMIZED);

		if (!maximized && changed)
		{
			glfwSetWindowPos(glfwWindow, newPosition.x, newPosition.y);
			glfwSetWindowSize(glfwWindow, newSize.x, newSize.y);
		}
	}

	bool EditorLayer::OnTitleBarHit(WindowTitleBarHitTestEvent& e)
	{
		e.SetHit(m_TitleBarHovered);
		return true;
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.GetRepeatCount() > 0 || m_SceneState == SceneState::Play)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		if (GImGui->ActiveId == 0)
		{
			if (m_ViewportHovered && m_SceneState == SceneState::Edit && !Input::IsMouseButtonPressed(Mouse::ButtonRight))
			{
				switch (e.GetKeyCode())
				{
					// Gizmos
					case Key::Q:
						m_GizmoType = -1;
						break;

					case Key::W:
						m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
						break;

					case Key::E:
						m_GizmoType = ImGuizmo::OPERATION::ROTATE;
						break;

					case Key::R:
						m_GizmoType = ImGuizmo::OPERATION::SCALE;
						break;

					// Grid
					case Key::G:
						m_SceneRenderer->GetOptions().ShowGrid = !m_SceneRenderer->GetOptions().ShowGrid;
						break;

					// Focus Camera
					case Key::F:
					{
						if (!m_ObjectsPanel.GetSelectedEntity())
							break;

						m_EditorCamera.Focus(m_ObjectsPanel.GetSelectedEntity().GetComponent<TransformComponent>().Position);
						break;
					}
				}
			}

			switch (e.GetKeyCode())
			{
				// Clear Selected Entity
				case Key::Space:
				{
					m_ObjectsPanel.SetSelectedEntity(Entity());
					m_ActiveScene->SetEditorSelectedEntity(Entity());
					break;
				}
				
				// Destroy Entity
				case Key::Delete:
				{
					if (m_ObjectsPanel.GetSelectedEntity())
					{
						m_ActiveScene->DestroyEntity(m_ObjectsPanel.GetSelectedEntity());
						m_ObjectsPanel.SetSelectedEntity(Entity());
						m_ActiveScene->SetEditorSelectedEntity(Entity());
						break;
					}
				}
			}	

		}

		switch (e.GetKeyCode())
		{
			// File Dialogs
			case Key::N:
			{
				if (control)
				{
					NewScene();
				}

				break;
			}

			case Key::O:
			{
				if (control)
				{
					OpenScene();
				}

				break;
			}

			case Key::S :
			{
				if (control && shift)
				{
					SaveSceneAs();
				}
				else if (control)
				{
					SaveScene();
				}

				break;
			}

			// Entity 
			case Key::D :
			{
				if (control && m_ObjectsPanel.GetSelectedEntity())
				{
					Entity duplicated = m_ActiveScene->DuplicateEntity(m_ObjectsPanel.GetSelectedEntity());
					m_ObjectsPanel.SetSelectedEntity(duplicated);
					m_ActiveScene->SetEditorSelectedEntity(duplicated);
					break;
				}
			}
		}
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_SceneState == SceneState::Edit)
			{
				if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftShift))
				{
					m_ObjectsPanel.SetSelectedEntity(m_HoveredEntity);
					m_ActiveScene->SetEditorSelectedEntity(m_HoveredEntity);
				}
			}
		}

		return false;
	}
}