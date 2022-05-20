#pragma once

#include "Renderer/Framebuffer.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Mesh.h"
#include "Scene/Scene.h"

namespace Venus {

	struct DrawCmd
	{
		Ref<Model> Model;
		glm::mat4 Transform;
	};

	class SceneRenderer
	{
		public:
			SceneRenderer(Ref<Scene> scene);

			void Init();
			void SetScene(Ref<Scene> scene);
			void SetViewportSize(uint32_t width, uint32_t height);

			void BeginScene(Camera& camera, const glm::mat4& transform);
			void BeginScene(EditorCamera& camera);
			void EndScene();

			void SubmitModel(Ref<Model> model, const glm::mat4& transform = glm::mat4(1.0f));
			void SubmitSelectedModel(Ref<Model> model, const glm::mat4& transform = glm::mat4(1.0f));

			Ref<Framebuffer> GetFinalRender();
			uint32_t GetFinalImage();

		private:
			void Flush();

			void GeometryPass();
			void CompositePass();

		private:
			Ref<Scene> m_Scene;

			std::vector<DrawCmd> m_DrawList;
			std::vector<DrawCmd> m_SelectedDrawList;
			
			// Pipelines
			Ref<Pipeline> m_GeometryPipeline;
			Ref<Pipeline> m_SelectedGeometryPipeline;
			Ref<Pipeline> m_GridPipeline;
			Ref<Pipeline> m_CompositePipeline;

			uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
			bool m_Rendering = false;
			bool m_NeedsResize = false;
	};

}

