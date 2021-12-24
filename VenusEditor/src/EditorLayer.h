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
			void OnOverlayRender();

			bool OnKeyPressed(KeyPressedEvent& e);
			bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

			void UpdateWindowTitle(const std::string& sceneName);
			void UpdateHoveredEntity();

			// UI
			void UI_ModalWelcome();
			void UI_MenuBar();
			void UI_ToolBar();
			void UI_Viewport();
			void UI_Settings();

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
			
			Ref<Texture2D> m_PlayIcon, m_StopIcon;
			Ref<Texture2D> m_GizmosPositionIcon, m_GizmosRotationIcon, m_GizmosScaleIcon;
			Ref<Texture2D> m_SceneCameraIcon;

			// Framebuffer
			Ref<Framebuffer> m_Framebuffer;

			// Scenes
			SceneState m_SceneState = SceneState::Edit;

			Ref<Scene> m_ActiveScene;
			Ref<Scene> m_EditorScene, m_RuntimeScene;

			EditorCamera m_EditorCamera;
			Entity m_HoveredEntity;
			bool m_CameraLocked = false;
			std::string m_ScenePath = std::string();

			// Gizmos
			int m_GizmoType = 0;

			// Viewport
			glm::vec4 m_ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
			bool m_ViewportFocused = false, m_ViewportHovered = false;
			glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
			glm::vec2 m_ViewportBounds[2];

			// Editor Settings
			bool m_ShowWelcomeMessage = true;
			bool m_ShowPhysicsColliderEditor = true;
			bool m_ShowPhysicsColliderRuntime = false;
			bool m_ShowCameraIcon = true;
	};
}