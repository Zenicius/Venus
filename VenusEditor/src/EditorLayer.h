#pragma once

#include "Venus.h"
#include "Panels/ObjectsPanel.h"

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
			// Panels
			ObjectsPanel m_ObjectsPanel;

			// Framebuffer
			Ref<Framebuffer> m_Framebuffer;

			// Scene 
			Ref<Scene> m_ActiveScene;
			OrthographicCameraController m_CameraController;

			// Viewport
			bool m_ViewportFocused = false, m_ViewportHovered = false;
			glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
			glm::vec4 m_ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	};
}