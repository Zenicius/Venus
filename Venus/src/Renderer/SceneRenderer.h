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
		Ref<Model> Model;
		glm::mat4 Transform;
		int ID;
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
		bool ShowGrid = true;

		// Shadows
		int ShadowMapDebugCascade = 0;
		float CascadeSplitLambda = 0.92f;
		float CascadeNearPlaneOffset = -50.0f;
		float CascadeFarPlaneOffset = 50.0f;

		// Bloom
		bool Bloom = false;
		int Bloom0Mip = 0;
		int Bloom1Mip = 0;
		int Bloom2Mip = 0;

		// Post
		bool FXAA = true;
		float Exposure = 1.0f;
		bool Grayscale = false;
		bool ACESTone = true;
		bool GammaCorrection = true;
	};

	class SceneRenderer
	{
		public:
			SceneRenderer(Ref<Scene> scene);

			void Init();
			void SetScene(Ref<Scene> scene);
			void SetViewportSize(uint32_t width, uint32_t height);

			void BeginScene(SceneCamera& camera, const glm::mat4& transform);
			void BeginScene(EditorCamera& camera);
			void EndScene();

			void SubmitModel(Ref<Model> model, const glm::mat4& transform = glm::mat4(1.0f), int entityID = -1);
			void SubmitSelectedModel(Ref<Model> model, const glm::mat4& transform = glm::mat4(1.0f), int entityID = -1);

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

			std::vector<DrawCmd> m_DrawList;
			std::vector<DrawCmd> m_SelectedDrawList;
			std::vector<DrawCmd> m_ShadowDrawList;

			uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
			bool m_Rendering = false;
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
			Ref<Pipeline> m_CompositePipeline;

			Ref<Pipeline> m_TempPipeline;
			Ref<Pipeline> m_BloomDebugPipeline[3];
			//------------------------------------------------------------


			//-- Materials------------------------------------------------
			Ref<Material> m_GridMaterial;
			Ref<Material> m_SkyboxMaterial;
			Ref<Material> m_FXAAMaterial;
			Ref<Material> m_BloomMaterial;
			Ref<Material> m_CompositeMaterial;

			Ref<Material> m_TempMaterial;
			Ref<Material> m_BloomDebugMaterial[3];
			//------------------------------------------------------------



			//-- Textures-------------------------------------------------
			Ref<Texture2D> m_BloomTextures[3];
			//------------------------------------------------------------

	};

}

