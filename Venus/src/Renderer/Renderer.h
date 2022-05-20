#pragma once

#include "Engine/Timer.h"

#include "Renderer/Pipeline.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Camera.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/Framebuffer.h"

#include "Scene/Components.h"

namespace Venus {

	class Renderer
	{
		public:
			static void Init();
			static void Shutdown();
			
			static void Clear(Ref<Framebuffer> framebuffer, glm::vec4 color = { 0.0f, 0.0f, 0.0f, 1.0f });
			static void OnWindowResize(uint32_t width, uint32_t height);

			static void BeginScene(const Camera& camera, const glm::mat4& transform);
			static void BeginScene(const EditorCamera& camera);
			static void EndScene();

			static void Render(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
			static void RenderQuad(const Ref<Pipeline>& pipeline, const glm::mat4& transform = glm::mat4(1.0f));
			static void RenderModel(const Ref<Pipeline>& pipeline, const Ref<Model>& model, const glm::mat4& transform = glm::mat4(1.0f), int entityID = -1);
			static void RenderSelectedModel(const Ref<Pipeline>& pipeline, const Ref<Model>& model, const glm::mat4& transform = glm::mat4(1.0f), int entityID = -1);

			static Ref<Texture2D> GetDefaultTexture();

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
