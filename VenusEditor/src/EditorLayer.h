#pragma once

#include <Venus.h>

#include "Panels/ObjectsPanel.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/RendererStatsPanel.h"

namespace Venus {

	class EditorLayer : public Layer
	{
		public:
			EditorLayer();
			virtual ~EditorLayer() = default;

			virtual void OnAttach() override;
			virtual void OnDetach() override;
			void OnUpdate(Timestep ts) override;
			virtual void OnImGuiRender() override;
			void OnEvent(Event& e) override;

		private:
			void NewScene();
			void OpenScene();
			void OpenScene(const std::filesystem::path& path);
			void SaveSceneAs();
			void SaveScene();

			void OnScenePlay();
			void OnSceneStop();

			bool OnManualWindowResize();
			bool OnTitleBarHit(WindowTitleBarHitTestEvent& e);
			bool OnKeyPressed(KeyPressedEvent& e);
			bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

			void UpdateWindowTitle(const std::string& sceneName);
			void UpdateHoveredEntity();

			// UI
			void UI_ModalWelcome();
			float UI_TitleBar();
			void UI_MenuBar();
			void UI_ToolBar();
			void UI_Viewport();
			void UI_Settings();
			void UI_WindowBorder();

		private:

			enum class SceneState
			{
				Edit = 0,
				Play = 1
			};

			// Panels
			ObjectsPanel m_ObjectsPanel;
			AssetBrowserPanel m_AssetBrowserPanel;
			RendererStatsPanel m_RendererStatsPanel;
			ConsolePanel m_ConsolePanel;
			
			// Icons
			Ref<Texture2D> m_VenusLogoIcon;
			Ref<Texture2D> m_PlayIcon, m_StopIcon;
			Ref<Texture2D> m_SceneCameraIcon;
			Ref<Texture2D> m_EditorCameraIcon;
			Ref<Texture2D> m_CreateIcon;
			Ref<Texture2D> m_SaveIcon, m_UndoIcon, m_RedoIcon;
			Ref<Texture2D> m_SettingsIcon;
			Ref<Texture2D> m_MinimizeIcon, m_MaximizeIcon, m_RestoreIcon, m_CloseIcon;

			// Scenes
			SceneState m_SceneState = SceneState::Edit;

			Ref<Scene> m_ActiveScene;
			Ref<Scene> m_EditorScene, m_RuntimeScene;

			Ref<SceneRenderer> m_SceneRenderer;

			EditorCamera m_EditorCamera;
			Entity m_HoveredEntity;
			bool m_CameraLocked = false;
			std::string m_ScenePath = std::string();

			// Gizmos
			int m_GizmoType = 0;

			// Viewport
			bool m_ViewportFocused = false, m_ViewportHovered = false;
			glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
			glm::vec2 m_ViewportBounds[2];

			// Editor Settings
			bool m_TitleBarHovered = false;

			bool m_ShowAssetBrowserPanel = true;
			bool m_ShowStatsPanel = false;
			bool m_ShowConsolePanel = true;
			bool m_ShowSceneSettingsPanel = true;
			bool m_ShowWelcomeMessage = false;
			
			bool m_ShowCameraIcon = true;
	};
}