#pragma once

#include "Engine/Timer.h"

#include "Renderer/Pipeline.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Camera.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Material.h"

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
			static void RenderQuadWithMaterial(const Ref<Pipeline>& pipeline, const glm::mat4& transform, const Ref<Material>& material); 
			static void RenderFullscreenQuad(const Ref<Pipeline>& pipeline, const Ref<Material>& material);
			static void RenderCube(const Ref<Pipeline>& pipeline, const Ref<Material>& material);
			static void RenderModel(const Ref<Pipeline>& pipeline, const Ref<Model>& model, const glm::mat4& transform = glm::mat4(1.0f), int entityID = -1);
			static void RenderModelWithMaterial(const Ref<Pipeline>& pipeline, const Ref<Model>& model, const glm::mat4& transform = glm::mat4(1.0f), int entityID = -1);
			static void RenderSelectedModel(const Ref<Pipeline>& pipeline, const Ref<Model>& model, const glm::mat4& transform = glm::mat4(1.0f), int entityID = -1);
			
			static void SetEnvironment(Ref<SceneEnvironment> envMap, uint32_t shadowMap);

			static Ref<SceneEnvironment> CreateEmptyEnvironmentMap();
			static Ref<SceneEnvironment> CreateEnvironmentMap(const std::string& path);
			static Ref<TextureCube> CreatePreethamSky(glm::vec3 TurbidityAzimuthInclination = { 2.0f, 0.0f, 0.0f });

			static Ref<Texture2D> GetDefaultTexture();
			static Ref<TextureCube> GetDefaultTextureCube();
			static Ref<ShaderLibrary> GetShaderLibrary();

			static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

			static void SetDebugTexture(uint32_t textureID);
			static float GetDebugParam();
			static void OnImGuiRender();

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
