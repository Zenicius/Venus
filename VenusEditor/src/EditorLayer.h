#pragma once

#include "Venus.h"
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

			void UpdateHoveredEntity();

			bool OnKeyPressed(KeyPressedEvent& e);
			bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		private:
			// Panels
			ObjectsPanel m_ObjectsPanel;
			AssetBrowserPanel m_AssetBrowserPanel;
			RendererStatsPanel m_RendererStatsPanel;

			// Framebuffer
			Ref<Framebuffer> m_Framebuffer;

			// Scene 
			EditorCamera m_EditorCamera;
			Entity m_HoveredEntity;
			bool m_CameraLocked = false;
			Ref<Scene> m_ActiveScene;
			std::string m_ScenePath = std::string();
			
			// Gizmos
			int m_GizmoType = 0;

			// Viewport
			glm::vec4 m_ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
			bool m_ViewportFocused = false, m_ViewportHovered = false;
			glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
			glm::vec2 m_ViewportBounds[2];
	};
}