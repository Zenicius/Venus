#pragma once

#include <Venus.h>

#include "Panels/ObjectsPanel.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/RendererStatsPanel.h"
#include "Panels/MaterialEditorPanel.h"

namespace Venus {

	class EditorLayer : public Layer
	{
		public:
			EditorLayer();
			virtual ~EditorLayer() = default;

			virtual void OnAttach() override;
			virtual void OnDetach() override;
			virtual void OnUpdate(Timestep ts) override;
			virtual void OnImGuiRender() override;
			virtual void OnEvent(Event& e) override;

		private:
			void NewScene();
			void OpenScene();
			void OpenScene(const std::filesystem::path& path);
			void SaveSceneAs();
			void SaveScene();

			void OnScenePlay();
			void OnSceneSimulate();
			void OnSceneStop();
			void OnOverlayRender();

			void OnAssetOpen(AssetType type, const std::filesystem::path& path);

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
				Play = 1,
				Simulate = 2,
			};

			// Panels
			ObjectsPanel m_ObjectsPanel;
			AssetBrowserPanel m_AssetBrowserPanel;
			RendererStatsPanel m_RendererStatsPanel;
			ConsolePanel m_ConsolePanel;
			MaterialEditorPanel m_MaterialEditorPanel;
			
			// Icons
			Ref<Texture2D> m_VenusLogoIcon;
			Ref<Texture2D> m_PlayIcon, m_SimulateIcon, m_StopIcon, m_PauseIcon, m_UnpauseIcon;
			Ref<Texture2D> m_DPlayIcon, m_DSimulateIcon, m_DStopIcon;
			Ref<Texture2D> m_SceneCameraIcon, m_PointLightIcon;
			Ref<Texture2D> m_CreateIcon;
			Ref<Texture2D> m_SaveIcon;
			Ref<Texture2D> m_SettingsIcon;
			Ref<Texture2D> m_MinimizeIcon, m_MaximizeIcon, m_RestoreIcon, m_CloseIcon;

			// Scenes
			SceneState m_SceneState = SceneState::Edit;

			Ref<Scene> m_ActiveScene;
			Ref<Scene> m_EditorScene, m_RuntimeScene;

			Ref<SceneRenderer> m_SceneRenderer;

			EditorCamera m_EditorCamera;
			Entity m_HoveredEntity;
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
			bool m_ShowAssetsInspector = false;
			bool m_ShowStatsPanel = false;
			bool m_ShowConsolePanel = true;
			bool m_ShowSceneSettingsPanel = false;
			bool m_ShowWelcomeMessage = false;
			bool m_ShowMaterialEditor = false;
			
			bool m_ShowIcons = true;
			bool m_ShowColliders = true;
			bool m_ShowLightRadius = true;
			bool m_ShowOverlayInRuntime = false;
	};
}