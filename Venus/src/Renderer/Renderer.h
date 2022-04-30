#pragma once

#include "Renderer/RenderCommand.h"

#include "Renderer/Camera.h"
#include "Renderer/OrthographicCamera.h"
#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/EditorCamera.h"

namespace Venus {

	class Renderer
	{
		public:
			static void Init();
			static void Shutdown();
		
			static void OnWindowResize(uint32_t width, uint32_t height);

			static void BeginScene(const Camera& camera, const glm::mat4& transform);
			static void BeginScene(const EditorCamera& camera);
			static void EndScene();

			static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
			static void SubmitModel(const Ref<Model>& model, const glm::mat4& transform = glm::mat4(1.0f), int entityID = -1);

			static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

			// Stats
			struct Statistics
			{
				uint32_t DrawCalls = 0;
				uint32_t VertexCount = 0;
				uint32_t IndexCount = 0;
			};
			static void ResetStats();
			static Statistics GetStats();
	};
}
