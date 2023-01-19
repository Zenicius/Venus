#pragma once

#include "Renderer/Framebuffer.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Mesh.h"
#include "Renderer/MeshMaterial.h"
#include "Renderer/UniformBuffer.h"
#include "Renderer/ComputePipeline.h"
#include "Scene/Scene.h"

namespace Venus {

	struct DrawCmd
	{
		glm::mat4 Transform;
		Ref<Model> Model;
		Ref<MaterialTable> MaterialTable;
		int ID;
	};

	struct QuadDrawCmd
	{
		glm::mat4 Transform;
		Ref<Texture2D> Texture;
		float TilingFactor;
		glm::vec4 TintColor;
		int ID;
	};

	struct CircleDrawCmd
	{
		glm::mat4 Transform;
		glm::vec4 Color;
		float Thickness;
		float Fade;
		int ID;
	};

	struct RectDrawCmd
	{
		glm::mat4 Transform;
		glm::vec4 Color;
		int ID;
	};

	struct BillboardDrawCmd
	{
		glm::vec3 Position;
		glm::vec2 Size;
		Ref<Texture2D> Texture;
		float TilingFactor;
		glm::vec4 TintColor;
	};

	struct CameraInfo
	{
		Camera Camera;
		glm::mat4 ViewMatrix;
		float Near;
		float Far;
		float FOV;
	};

	struct SceneRendererOptions
	{
		// Geral
		bool ShowGrid = true;

		// Shadows
		int ShadowMapDebugCascade = 0;
		float CascadeSplitLambda = 0.92f;
		float CascadeNearPlaneOffset = -50.0f;
		float CascadeFarPlaneOffset = 50.0f;

		// Bloom
		bool Bloom = true;
		float BloomIntensity = 1.0f;
		float BloomThreshold = 1.0f;
		float BloomKnee = 0.1f;
		AssetHandle BloomDirtMask = 0;
		float BloomDirtMaskIntensity = 1.0f;
		int BloomDebugTex = 0;
		int BloomDebugMip = 0;

		// Color / Lightning
		float Exposure = 1.0f;
		bool Grayscale = false;
		bool ACESTone = true;
		bool GammaCorrection = true;

		// Anti Aliasing
		bool FXAA = true;
		float FXAAThresholdMin = 32.0f;
		float FXAAThresholdMax = 8.0f;
		int FXAAIterations = 12;
		float FXAASubPixelQuality = 0.75f;
	};

	class SceneRenderer
	{
		public:
			SceneRenderer(Ref<Scene> scene);

			void Init();
			void SetScene(Ref<Scene> scene);
			void SetViewportSize(uint32_t width, uint32_t height);

			void BeginScene(CameraComponent& cameraComponent, const glm::mat4& transform);
			void BeginScene(EditorCamera& camera);
			void EndScene();

			void SubmitModel(const Ref<Model>& model, const Ref<MaterialTable>& materialTable, const glm::mat4& transform = glm::mat4(1.0f), int entityID = -1);
			void SubmitSelectedModel(const Ref<Model>& model, const Ref<MaterialTable>& materialTable, const glm::mat4& transform = glm::mat4(1.0f), int entityID = -1);
			void SubmitQuad(const glm::mat4& transform, const Ref<Texture2D>& texture = nullptr, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), int entityID = -1);
			void SubmitCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);
			void SubmitRect(const glm::mat4& transform, const glm::vec4 color, int entityID = -1);
			void SubmitBillboard(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

			SceneRendererOptions& GetOptions() { return m_Options; }
			void OnImGuiRender(bool& show);

			Ref<Framebuffer> GetGeometryBuffer();
			Ref<Framebuffer> GetFinalBuffer();
			uint32_t GetFinalImage();

		private:
			void Flush();

			void ShadowMapPass();
			void GeometryPass();
			void FXAAPass();
			void BloomPass();
			void CompositePass();
			void Render2DPass();


			//-- Shadows Cascade
			struct CascadeData
			{
				glm::mat4 ViewProj;
				glm::mat4 View;
				float SplitDepth;
			};
			void CalculateCascades(CascadeData* cascades, const CameraInfo& camera, const glm::vec3& lightDirection);

		private:
			Ref<Scene> m_Scene;

			SceneRendererOptions m_Options;

			CameraComponent* m_RuntimeCamera = nullptr;

			std::vector<DrawCmd> m_DrawList;
			std::vector<DrawCmd> m_SelectedDrawList;
			std::vector<DrawCmd> m_ShadowDrawList;

			std::vector<QuadDrawCmd> m_QuadDrawList;
			std::vector<CircleDrawCmd> m_CircleDrawList;
			std::vector<RectDrawCmd> m_RectDrawList;
			std::vector<BillboardDrawCmd> m_BillboardDrawList;

			uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
			glm::vec4 m_EditorBackgroundColor = { 0.1f, 0.1f, 0.1f, 1.0f };
			bool m_Rendering = false;
			bool m_IsRuntime = false;
			bool m_NeedsResize = false;


			//-- Shadow Data----------------------------------------------
			struct ShadowData
			{
				glm::mat4 ViewProjection[4];
			};
			ShadowData m_ShadowBuffer;
			Ref<UniformBuffer> m_ShadowDataBuffer;
			//------------------------------------------------------------



			//-- Point Light Data-----------------------------------------
			struct PointLightData
			{
				uint32_t Count{ 0 };
				glm::vec3 Padding{};
				PointLight PointLights[1024]{};
			};
			PointLightData m_PointLightBuffer;
			Ref<UniformBuffer> m_PointLightDataBuffer;
			//------------------------------------------------------------



			//-- Scene Data-----------------------------------------------
			struct DirLight												
			{
				glm::vec3 Direction;
				float HasDirLight = 0; // Also padding
				glm::vec3 Color;
				float Intensity;
			};
			struct SceneData
			{
				DirLight DirLight;
				glm::vec3 u_CameraPosition;
				float EnvironmentMapIntensity = 1.0f;
			};
			SceneData m_SceneBuffer;
			Ref<UniformBuffer> m_SceneDataBuffer;
			//------------------------------------------------------------



			//-- Renderer Data--------------------------------------------
			struct RendererData
			{
				glm::vec4 CascadeSplits;
				uint32_t TilesCountX{ 0 };
				bool ShowCascades = false;
				char Padding0[3] = { 0,0,0 };
				bool SoftShadows = true;
				char Padding1[3] = { 0,0,0 };
				float LightSize = 0.5f;
				float MaxShadowDistance = 200.0f;
				float ShadowFade = 1.0f;
				bool CascadeFading = true;
				char Padding2[3] = { 0,0,0 };
				float CascadeTransitionFade = 1.0f;
				bool ShowLightComplexity = false;
				char Padding3[3] = { 0,0,0 };
			};
			RendererData m_RendererBuffer;
			Ref<UniformBuffer> m_RendererDataBuffer;
			//------------------------------------------------------------

			

			//-- Pipelines------------------------------------------------
			Ref<Pipeline> m_ShadowPipeline;
			Ref<Pipeline> m_GeometryPipeline;
			Ref<Pipeline> m_SelectedGeometryPipeline;
			Ref<Pipeline> m_GridPipeline;
			Ref<Pipeline> m_SkyboxPipeline;

			Ref<Pipeline> m_FXAAPipeline;
			Ref<ComputePipeline> m_BloomPipeline;
			Ref<Framebuffer> m_2DFramebuffer;
			Ref<Pipeline> m_CompositePipeline;

			Ref<Pipeline> m_TempPipeline;
			Ref<Pipeline> m_BloomDebugPipeline;
			//------------------------------------------------------------


			//-- Materials------------------------------------------------
			Ref<Material> m_GridMaterial;
			Ref<Material> m_SkyboxMaterial;
			Ref<Material> m_FXAAMaterial;
			Ref<Material> m_BloomMaterial;
			Ref<Material> m_CompositeMaterial;

			Ref<Material> m_TempMaterial;
			Ref<Material> m_BloomDebugMaterial;
			//------------------------------------------------------------



			//-- Textures-------------------------------------------------
			Ref<Texture2D> m_BloomTextures[3];
			//------------------------------------------------------------

	};

}

