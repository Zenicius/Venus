#include "pch.h"
#include "SceneRenderer.h"

#include "Renderer.h"
#include "Renderer2D.h"
#include "Engine/Application.h"
#include "Assets/AssetManager.h"

#include "ImGui/UI.h"

namespace Venus {

	SceneRenderer::SceneRenderer(Ref<Scene> scene)
		:m_Scene(scene)
	{
		Init();
	}

	void SceneRenderer::Init()
	{
		// Shadows
		{
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::DEPTH32F };
			fbSpec.Samples = 1;
			fbSpec.Layers = 4; // Cascades
			fbSpec.SwapChainTarget = false;
			fbSpec.Width = 4096;
			fbSpec.Height = 4096;
			Ref<Framebuffer> framebuffer = Framebuffer::Create(fbSpec);

			PipelineSpecification pipelineSpec;
			pipelineSpec.Shader = Renderer::GetShaderLibrary()->Get("ShadowMap");
			pipelineSpec.Framebuffer = framebuffer;
			pipelineSpec.DebugName = "Shadow Map";

			m_ShadowPipeline = Pipeline::Create(pipelineSpec);
		}

		// Geometry 
		{
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA32F, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::DEPTH24STENCIL8 };
			fbSpec.Samples = 1;
			fbSpec.SwapChainTarget = false;
			fbSpec.Width = 1920;
			fbSpec.Height = 1080;
			Ref<Framebuffer> framebuffer = Framebuffer::Create(fbSpec);

			PipelineSpecification pipelineSpec;
			pipelineSpec.Shader = Renderer::GetShaderLibrary()->Get("PBR");
			pipelineSpec.Framebuffer = framebuffer;
			pipelineSpec.DebugName = "Geometry";

			m_GeometryPipeline = Pipeline::Create(pipelineSpec);
		}

		// Selected Geometry
		{
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA32F, FramebufferTextureFormat::DEPTH24STENCIL8 };
			fbSpec.Samples = 1;
			fbSpec.SwapChainTarget = false;
			fbSpec.Width = 1920;
			fbSpec.Height = 1080;
			Ref<Framebuffer> framebuffer = Framebuffer::Create(fbSpec);

			PipelineSpecification pipelineSpec;
			pipelineSpec.Shader = Renderer::GetShaderLibrary()->Get("Outline");
			pipelineSpec.Framebuffer = framebuffer;
			pipelineSpec.DebugName = "Selected Geometry";

			m_SelectedGeometryPipeline = Pipeline::Create(pipelineSpec);
		}

		// Grid
		{
			PipelineSpecification pipelineSpec;
			pipelineSpec.Shader = Renderer::GetShaderLibrary()->Get("Grid");
			pipelineSpec.Framebuffer = m_GeometryPipeline->GetFramebuffer();
			pipelineSpec.DebugName = "Grid";

			m_GridPipeline = Pipeline::Create(pipelineSpec);
			
			m_GridMaterial = Material::Create(pipelineSpec.Shader);
			m_GridMaterial->SetFloat("u_Settings.Scale", 16.025f);
			m_GridMaterial->SetFloat("u_Settings.Size", 0.025f);
		}

		// Skybox
		{
			PipelineSpecification pipelineSpec;
			pipelineSpec.Shader = Renderer::GetShaderLibrary()->Get("Skybox");
			pipelineSpec.Framebuffer = m_GeometryPipeline->GetFramebuffer();
			pipelineSpec.DebugName = "Skybox";

			m_SkyboxPipeline = Pipeline::Create(pipelineSpec);

			m_SkyboxMaterial = Material::Create(pipelineSpec.Shader);
		}

		// FXAA 
		{
			FramebufferSpecification fbSpec;
			fbSpec.SwapChainTarget = false;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA32F };
			fbSpec.Width = 1920;
			fbSpec.Height = 1080;
			Ref<Framebuffer> framebuffer = Framebuffer::Create(fbSpec);

			PipelineSpecification pipelineSpec;
			pipelineSpec.Shader = Renderer::GetShaderLibrary()->Get("FXAA");
			pipelineSpec.Framebuffer = framebuffer;
			pipelineSpec.DebugName = "FXAA";

			m_FXAAPipeline = Pipeline::Create(pipelineSpec);

			m_FXAAMaterial = Material::Create(pipelineSpec.Shader);
		}

		// Bloom
		{
			Ref<Shader> bloomShader = Renderer::GetShaderLibrary()->Get("Bloom");
			m_BloomPipeline = ComputePipeline::Create(bloomShader);
			m_BloomMaterial = Material::Create(bloomShader);

			m_BloomTextures[0] = Texture2D::Create(1, 1);
			m_BloomTextures[1] = Texture2D::Create(1, 1);
			m_BloomTextures[2] = Texture2D::Create(1, 1);
		}

		// Scene Composite
		{
			FramebufferSpecification fbSpec;
			fbSpec.SwapChainTarget = false;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
			fbSpec.Width = 1920;
			fbSpec.Height = 1080;
			Ref<Framebuffer> framebuffer = Framebuffer::Create(fbSpec);

			PipelineSpecification pipelineSpec;
			pipelineSpec.Shader = Renderer::GetShaderLibrary()->Get("Composite");
			pipelineSpec.Framebuffer = framebuffer;
			pipelineSpec.DebugName = "Composite";

			m_CompositePipeline = Pipeline::Create(pipelineSpec);

			m_CompositeMaterial = Material::Create(pipelineSpec.Shader);
		}

		// Temp Pipeline 
		{
			FramebufferSpecification fbSpec;
			fbSpec.SwapChainTarget = false;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
			fbSpec.Width = 1920;
			fbSpec.Height = 1080;
			Ref<Framebuffer> framebuffer = Framebuffer::Create(fbSpec);

			PipelineSpecification pipelineSpec;
			pipelineSpec.Shader = Renderer::GetShaderLibrary()->Get("ShadowMapDebug");
			pipelineSpec.Framebuffer = framebuffer;
			pipelineSpec.DebugName = "Temp";

			m_TempPipeline = Pipeline::Create(pipelineSpec);

			m_TempMaterial = Material::Create(pipelineSpec.Shader);
		}

		//  Bloom Debug
		{
			FramebufferSpecification fbSpec;
			fbSpec.SwapChainTarget = false;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA8 };
			fbSpec.Width = 1920;
			fbSpec.Height = 1080;
			Ref<Framebuffer> framebuffer = Framebuffer::Create(fbSpec);

			PipelineSpecification pipelineSpec;
			pipelineSpec.Shader = Renderer::GetShaderLibrary()->Get("BloomDebug");
			pipelineSpec.Framebuffer = framebuffer;
			pipelineSpec.DebugName = "Bloom Debug";

			m_BloomDebugPipeline = Pipeline::Create(pipelineSpec);

			m_BloomDebugMaterial = Material::Create(pipelineSpec.Shader);
		}

		// 2D
		{
			FramebufferSpecification fbSpec;
			fbSpec.SwapChainTarget = false;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::DEPTH24STENCIL8 };
			fbSpec.Width = 1920;
			fbSpec.Height = 1080;
			fbSpec.ExistingColorTextures.push_back(m_CompositePipeline->GetFramebuffer()->GetColorAttachmentRendererID());
			fbSpec.ExistingDepthTexture = m_GeometryPipeline->GetFramebuffer()->GetDepthAttachmentRendererID();

			m_2DFramebuffer = Framebuffer::Create(fbSpec);

			Renderer2D::SetRenderTarget(m_2DFramebuffer);
		}

		// Uniform Buffers
		m_ShadowDataBuffer = UniformBuffer::Create(sizeof(ShadowData), 2);
		m_SceneDataBuffer = UniformBuffer::Create(sizeof(SceneData), 3);
		m_PointLightDataBuffer = UniformBuffer::Create(sizeof(PointLightData), 4);
		m_RendererDataBuffer = UniformBuffer::Create(sizeof(RendererData), 5);
	}

	void SceneRenderer::SetScene(Ref<Scene> scene)
	{
		VS_CORE_ASSERT(!m_Rendering, "Tried to change Scene while rendering!");

		if(!m_Rendering)
			m_Scene = scene;
	}

	void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
	{
		if (m_ViewportWidth != width || m_ViewportHeight != height)
		{
			m_ViewportWidth = width;
			m_ViewportHeight = height;
			m_NeedsResize = true;

		}
	}

	void SceneRenderer::BeginScene(CameraComponent& cameraComponent, const glm::mat4& transform)
	{
		// Resizes if needed
		if (m_NeedsResize)
		{
			m_GeometryPipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
			m_SelectedGeometryPipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
			m_FXAAPipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
			m_CompositePipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
			m_TempPipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
			m_BloomDebugPipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);

			// Update Existing Textures Renderer2D framebuffer
			{
				uint32_t colorTexture = m_CompositePipeline->GetFramebuffer()->GetColorAttachmentRendererID();
				uint32_t depthTexture = m_GeometryPipeline->GetFramebuffer()->GetDepthAttachmentRendererID();

				m_2DFramebuffer->GetSpecification().ExistingColorTextures.clear();
				m_2DFramebuffer->GetSpecification().ExistingColorTextures.push_back(colorTexture);
				m_2DFramebuffer->GetSpecification().ExistingDepthTexture = depthTexture;

				m_2DFramebuffer->Resize(m_ViewportWidth, m_ViewportHeight);
			}

			// Bloom Textures
			{
				TextureProperties props;
				props.Format = TextureFormat::RGBA32F;
				props.WrapMode = TextureWrapMode::ClampToEdge;
				props.UseMipmaps = true;

				uint32_t viewportWidth = m_ViewportWidth / 2;
				uint32_t viewportHeight = m_ViewportHeight / 2;
				viewportWidth += (4 - (viewportWidth % 4)); // Workgroup size = 4
				viewportHeight += (4 - (viewportHeight % 4));

				m_BloomTextures[0] = Texture2D::Create(viewportWidth, viewportHeight, props);
				m_BloomTextures[1] = Texture2D::Create(viewportWidth, viewportHeight, props);
				m_BloomTextures[2] = Texture2D::Create(viewportWidth, viewportHeight, props);
			}

			m_NeedsResize = false;
		}

		// Clear buffers
		Renderer::Clear(m_ShadowPipeline->GetFramebuffer());
		Renderer::Clear(m_GeometryPipeline->GetFramebuffer(), cameraComponent.BackgroundColor);
		m_GeometryPipeline->GetFramebuffer()->ClearAttachment(1, -1);
		Renderer::Clear(m_SelectedGeometryPipeline->GetFramebuffer());
		Renderer::Clear(m_FXAAPipeline->GetFramebuffer());
		Renderer::Clear(m_CompositePipeline->GetFramebuffer());
		Renderer::Clear(m_TempPipeline->GetFramebuffer());
		Renderer::Clear(m_BloomDebugPipeline->GetFramebuffer());

		Renderer::ResetStats();

		// Begin scene at Renderer
		const SceneCamera& camera = cameraComponent.Camera;
		Renderer::BeginScene(camera, transform);
		glm::mat4 cameraViewMatrix = glm::inverse(transform);
		m_IsRuntime = true;
		m_RuntimeCamera = &cameraComponent;

		// Set Uniform Buffers
		const auto& dirLight = m_Scene->m_LightEnvironment.DirectionalLight;
		m_SceneBuffer.DirLight.Direction = dirLight.Direction;
		m_SceneBuffer.DirLight.Color = dirLight.Color;
		m_SceneBuffer.DirLight.Intensity = dirLight.Intensity;
		m_SceneBuffer.DirLight.HasDirLight = m_Scene->m_LightEnvironment.HasDirLight;
		m_SceneBuffer.u_CameraPosition = glm::inverse(cameraViewMatrix)[3];
		m_SceneBuffer.EnvironmentMapIntensity = m_Scene->m_LightEnvironment.SkyLight.Intensity;
		m_SceneDataBuffer->SetData(&m_SceneBuffer, sizeof(m_SceneBuffer)); // SCENE BUFFER

		const std::vector<PointLight>& pointLightsVec = m_Scene->m_LightEnvironment.PointLights;
		m_PointLightBuffer.Count = uint32_t(pointLightsVec.size());
		std::memcpy(m_PointLightBuffer.PointLights, pointLightsVec.data(), sizeof PointLight * pointLightsVec.size());
		m_PointLightDataBuffer->SetData(&m_PointLightBuffer, 16ull + sizeof(PointLight) * m_PointLightBuffer.Count); // POINT LIGHT BUFFER

		CameraInfo cameraInfo;
		cameraInfo.Camera = camera;
		cameraInfo.ViewMatrix = cameraViewMatrix;
		cameraInfo.Near = camera.GetPerspectiveNearClip();
		cameraInfo.Far = camera.GetPerspectiveFarClip();
		cameraInfo.FOV = camera.GetPerspectiveVerticalFOV();
		CascadeData cascades[4];
		CalculateCascades(cascades, cameraInfo, dirLight.Direction);
		glm::vec4 cascadeSplit;
		for (uint32_t i = 0; i < 4; i++)
		{
			cascadeSplit[i] = cascades[i].SplitDepth;
			m_ShadowBuffer.ViewProjection[i] = cascades[i].ViewProj;
		}
		m_ShadowDataBuffer->SetData(&m_ShadowBuffer, sizeof(m_ShadowBuffer)); // SHADOW BUFFER

		m_RendererBuffer.CascadeSplits = cascadeSplit;
		m_RendererDataBuffer->SetData(&m_RendererBuffer, sizeof(m_RendererBuffer)); // RENDERER BUFFER

		// Environment
		uint32_t shadowMap = m_ShadowPipeline->GetFramebuffer()->GetDepthAttachmentRendererID();
		if (m_Scene->m_LightEnvironment.SkyLight.EnvironmentMap)
			Renderer::SetEnvironment(m_Scene->m_LightEnvironment.SkyLight.EnvironmentMap, shadowMap);
		else
			Renderer::SetEnvironment(Renderer::CreateEmptyEnvironmentMap(), shadowMap);
	}

	void SceneRenderer::BeginScene(EditorCamera& camera)
	{
		// Resizes if needed
		if (m_NeedsResize)
		{
			m_GeometryPipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
			m_SelectedGeometryPipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
			m_FXAAPipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
			m_CompositePipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
			m_TempPipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
			m_BloomDebugPipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);

			// Update Existing Textures Renderer2D framebuffer
			{
				uint32_t colorTexture = m_CompositePipeline->GetFramebuffer()->GetColorAttachmentRendererID();
				uint32_t depthTexture = m_GeometryPipeline->GetFramebuffer()->GetDepthAttachmentRendererID();

				m_2DFramebuffer->GetSpecification().ExistingColorTextures.clear();
				m_2DFramebuffer->GetSpecification().ExistingColorTextures.push_back(colorTexture);
				m_2DFramebuffer->GetSpecification().ExistingDepthTexture = depthTexture;

				m_2DFramebuffer->Resize(m_ViewportWidth, m_ViewportHeight);
			}

			// Bloom Textures
			{
				TextureProperties props;
				props.Format = TextureFormat::RGBA32F;
				props.WrapMode = TextureWrapMode::ClampToEdge;
				props.UseMipmaps = true;

				uint32_t viewportWidth = m_ViewportWidth / 2;
				uint32_t viewportHeight = m_ViewportHeight / 2;
				viewportWidth += (4 - (viewportWidth % 4)); // Workgroup size = 4
				viewportHeight += (4 - (viewportHeight % 4));

				m_BloomTextures[0] = Texture2D::Create(viewportWidth, viewportHeight, props);
				m_BloomTextures[1] = Texture2D::Create(viewportWidth, viewportHeight, props);
				m_BloomTextures[2] = Texture2D::Create(viewportWidth, viewportHeight, props);
			}

			m_NeedsResize = false;
		}

		// Clear buffers
		Renderer::Clear(m_ShadowPipeline->GetFramebuffer());
		Renderer::Clear(m_GeometryPipeline->GetFramebuffer(), m_EditorBackgroundColor);
		m_GeometryPipeline->GetFramebuffer()->ClearAttachment(1, -1);
		Renderer::Clear(m_SelectedGeometryPipeline->GetFramebuffer());
		Renderer::Clear(m_FXAAPipeline->GetFramebuffer());
		Renderer::Clear(m_CompositePipeline->GetFramebuffer());
		Renderer::Clear(m_TempPipeline->GetFramebuffer());
		Renderer::Clear(m_BloomDebugPipeline->GetFramebuffer());
		
		Renderer::ResetStats();

		// Begin scene at Renderer
		Renderer::BeginScene(camera);
		m_IsRuntime = false;

		// Set Uniform Buffers
		const auto& dirLight = m_Scene->m_LightEnvironment.DirectionalLight;
		m_SceneBuffer.DirLight.Direction = dirLight.Direction;
		m_SceneBuffer.DirLight.Color = dirLight.Color;
		m_SceneBuffer.DirLight.Intensity = dirLight.Intensity;
		m_SceneBuffer.DirLight.HasDirLight = m_Scene->m_LightEnvironment.HasDirLight;
		m_SceneBuffer.u_CameraPosition = glm::inverse(camera.GetViewMatrix())[3];
		m_SceneBuffer.EnvironmentMapIntensity = m_Scene->m_LightEnvironment.SkyLight.Intensity;
		m_SceneDataBuffer->SetData(&m_SceneBuffer, sizeof(m_SceneBuffer)); // SCENE BUFFER

		const std::vector<PointLight>& pointLightsVec = m_Scene->m_LightEnvironment.PointLights;
		m_PointLightBuffer.Count = uint32_t(pointLightsVec.size());
		std::memcpy(m_PointLightBuffer.PointLights, pointLightsVec.data(), sizeof PointLight * pointLightsVec.size());
		m_PointLightDataBuffer->SetData(&m_PointLightBuffer, 16ull + sizeof(PointLight) * m_PointLightBuffer.Count); // POINT LIGHT BUFFER

		CameraInfo cameraInfo;
		cameraInfo.Camera = camera;
		cameraInfo.ViewMatrix = camera.GetViewMatrix();
		cameraInfo.Near = camera.GetNearClip();
		cameraInfo.Far = camera.GetFarClip(); 
		cameraInfo.FOV = camera.GetVerticalFOV();
		CascadeData cascades[4];
		CalculateCascades(cascades, cameraInfo, dirLight.Direction);
		glm::vec4 cascadeSplit;
		for (uint32_t i = 0; i < 4; i++)
		{
			cascadeSplit[i] = cascades[i].SplitDepth;
			m_ShadowBuffer.ViewProjection[i] = cascades[i].ViewProj;
		}
		m_ShadowDataBuffer->SetData(&m_ShadowBuffer, sizeof(m_ShadowBuffer)); // SHADOW BUFFER

		m_RendererBuffer.CascadeSplits = cascadeSplit;
		m_RendererDataBuffer->SetData(&m_RendererBuffer, sizeof(m_RendererBuffer)); // RENDERER BUFFER

		// Environment
		uint32_t shadowMap = m_ShadowPipeline->GetFramebuffer()->GetDepthAttachmentRendererID();
		if (m_Scene->m_LightEnvironment.SkyLight.EnvironmentMap)
			Renderer::SetEnvironment(m_Scene->m_LightEnvironment.SkyLight.EnvironmentMap, shadowMap);
		else
			Renderer::SetEnvironment(Renderer::CreateEmptyEnvironmentMap(), shadowMap);
	}

	void SceneRenderer::EndScene()
	{
		m_Rendering = true;
		{
			Flush();
			Renderer::EndScene();
		}
		m_Rendering = false;
	}

	void SceneRenderer::SubmitModel(const Ref<Model>& model, const Ref<MaterialTable>& materialTable, const glm::mat4& transform, int entityID)
	{
		DrawCmd drawCmd;
		drawCmd.Model = model;
		drawCmd.MaterialTable = materialTable;
		drawCmd.Transform = transform;
		drawCmd.ID = entityID;

		m_DrawList.push_back(drawCmd);
		m_ShadowDrawList.push_back(drawCmd);
	}

	void SceneRenderer::SubmitSelectedModel(const Ref<Model>& model, const Ref<MaterialTable>& materialTable, const glm::mat4& transform, int entityID)
	{
		DrawCmd drawCmd;
		drawCmd.Model = model;
		drawCmd.MaterialTable = materialTable;
		drawCmd.Transform = transform;
		drawCmd.ID = entityID;

		m_DrawList.push_back(drawCmd);
		m_ShadowDrawList.push_back(drawCmd);
		//m_SelectedDrawList.push_back(drawCmd);
	}

	void SceneRenderer::SubmitQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		QuadDrawCmd drawCmd;
		drawCmd.Transform = transform;
		drawCmd.Texture = texture;
		drawCmd.TilingFactor = tilingFactor;
		drawCmd.TintColor = tintColor;
		drawCmd.ID = entityID;

		m_QuadDrawList.push_back(drawCmd);
	}

	void SceneRenderer::SubmitCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
	{
		CircleDrawCmd drawCmd;
		drawCmd.Transform = transform;
		drawCmd.Color = color;
		drawCmd.Thickness = thickness;
		drawCmd.Fade = fade;
		drawCmd.ID = entityID;

		m_CircleDrawList.push_back(drawCmd);
	}

	void SceneRenderer::SubmitRect(const glm::mat4& transform, const glm::vec4 color, int entityID)
	{
		RectDrawCmd drawCmd;
		drawCmd.Transform = transform;
		drawCmd.Color = color;
		drawCmd.ID = entityID;

		m_RectDrawList.push_back(drawCmd);
	}

	void SceneRenderer::SubmitBillboard(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		BillboardDrawCmd drawCmd;
		drawCmd.Position = position;
		drawCmd.Size = size;
		drawCmd.Texture = texture;
		drawCmd.TilingFactor = tilingFactor;
		drawCmd.TintColor = tintColor;

		m_BillboardDrawList.push_back(drawCmd);
	}

	void SceneRenderer::Flush()
	{
		ShadowMapPass();
		GeometryPass();
		FXAAPass();
		BloomPass();
		CompositePass();
		Render2DPass();

		m_DrawList.clear();
		m_SelectedDrawList.clear();
		m_ShadowDrawList.clear();

		m_QuadDrawList.clear();
		m_CircleDrawList.clear();
		m_RectDrawList.clear();
		m_BillboardDrawList.clear();
	}

	void SceneRenderer::ShadowMapPass()
	{
		if (m_Scene->m_LightEnvironment.HasDirLight && m_Scene->m_LightEnvironment.CastsShadows)
		{
			for (auto& cmd : m_ShadowDrawList)
			{
				Renderer::RenderModel(m_ShadowPipeline, cmd.Model, cmd.Transform);
			}
			
			// Shadow Map cascade viewer at settings
			{
				uint32_t depthTextureID = m_ShadowPipeline->GetFramebuffer()->GetDepthAttachmentRendererID();
				uint32_t layer = GetOptions().ShadowMapDebugCascade;
				m_TempMaterial->SetTextureArray("u_Texture", 0, depthTextureID);
				m_TempMaterial->SetInt("u_Settings.Layer", layer);
				Renderer::RenderFullscreenQuad(m_TempPipeline, m_TempMaterial);
			}
		}
	}

	void SceneRenderer::GeometryPass()
	{
		// Skybox
		{
			if (m_Scene->m_LightEnvironment.SkyLight.EnvironmentMap)
			{
				auto& skyLight = m_Scene->m_LightEnvironment.SkyLight;
				auto& envMap = skyLight.EnvironmentMap;

				m_SkyboxMaterial->SetFloat("u_Uniforms.TextureLod", skyLight.Lod);
				m_SkyboxMaterial->SetFloat("u_Uniforms.Intensity", skyLight.Intensity);
				m_SkyboxMaterial->SetCubeMap("u_Texture", 0, envMap->GetRadianceMap()->GetRendererID());

				Renderer::RenderCube(m_SkyboxPipeline, m_SkyboxMaterial);
			}
		}

		// Models
		{
			for (auto& cmd : m_DrawList)
			{
				Renderer::RenderModelWithMaterial(m_GeometryPipeline, cmd.Model, cmd.MaterialTable, cmd.Transform, cmd.ID);
			}
		}

		// Grid
		{
			if (GetOptions().ShowGrid && !m_IsRuntime)
			{
				const glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(8.0f));
				Renderer::RenderQuadWithMaterial(m_GridPipeline, transform, m_GridMaterial);
			}
		}
	}

	void SceneRenderer::FXAAPass()
	{
		if (!GetOptions().FXAA)
			return;

		auto& geoFramebuffer = m_GeometryPipeline->GetFramebuffer();
		uint32_t texture = geoFramebuffer->GetColorAttachmentRendererID();

		m_FXAAMaterial->SetInt("u_Settings.ViewportWidth", m_ViewportWidth);
		m_FXAAMaterial->SetInt("u_Settings.ViewportHeight", m_ViewportHeight);
		m_FXAAMaterial->SetFloat("u_Settings.ThresholdMin", GetOptions().FXAAThresholdMin);
		m_FXAAMaterial->SetFloat("u_Settings.ThresholdMax", GetOptions().FXAAThresholdMax);
		m_FXAAMaterial->SetInt("u_Settings.Iterations", GetOptions().FXAAIterations);
		m_FXAAMaterial->SetFloat("u_Settings.SubpixelQuality", GetOptions().FXAASubPixelQuality);
		m_FXAAMaterial->SetTexture("u_Texture", 0, texture);

		Renderer::RenderFullscreenQuad(m_FXAAPipeline, m_FXAAMaterial);
	}

	void SceneRenderer::BloomPass()
	{
		if (m_IsRuntime && !m_RuntimeCamera->UseRendererSettings && !m_RuntimeCamera->Bloom)
			return;
		else if (!GetOptions().Bloom)
			return;

		uint32_t workGroupSize = 4;

		float threshold = GetOptions().BloomThreshold;
		float knee = GetOptions().BloomKnee;
		int mode = 0;  // 0 Prefilter, 1 Downsample, 2 UpSample(1), 3 Upsample
		float LOD = 0.0f;

		auto inputTexture = m_GeometryPipeline->GetFramebuffer()->GetColorAttachmentRendererID();
		glm::vec4 params = { threshold, threshold - knee, knee * 2.0f, 0.25f / knee };

		uint32_t workGroupsX = 1, workGroupsY = 1;

		// Prefilter
		{
			mode = 0;
			LOD = 0.0f;

			workGroupsX = m_BloomTextures[0]->GetWidth() / workGroupSize;
			workGroupsY = m_BloomTextures[0]->GetHeight() / workGroupSize;

			m_BloomPipeline->Begin();
			m_BloomPipeline->GetShader()->SetFloat4("u_Uniforms.Params", params);
			m_BloomPipeline->GetShader()->SetInt("u_Uniforms.Mode", mode);
			m_BloomPipeline->GetShader()->SetFloat("u_Uniforms.LOD", LOD);
			m_BloomPipeline->BindWriteOnlyImage(0, m_BloomTextures[0]);
			m_BloomPipeline->GetShader()->SetTexture("u_Texture", 1, inputTexture);
			m_BloomPipeline->GetShader()->SetTexture("u_BloomTexture", 2, inputTexture);
			m_BloomPipeline->Execute(workGroupsX, workGroupsY, 1, false);
		}

		// Downsample
		int mipCount = m_BloomTextures[0]->GetMipLevelCount() - 2;
		{
			mode = 1;

			for (int i = 1; i < mipCount; i++)
			{
				LOD = i - 1.0f;

				auto [mipWidth, mipHeight] = m_BloomTextures[0]->GetMipSize(i);
				workGroupsX = (uint32_t)glm::ceil((float)mipWidth / (float)workGroupSize);
				workGroupsY = (uint32_t)glm::ceil((float)mipHeight / (float)workGroupSize);

				m_BloomPipeline->Begin();
				m_BloomPipeline->GetShader()->SetFloat4("u_Uniforms.Params", params);
				m_BloomPipeline->GetShader()->SetInt("u_Uniforms.Mode", mode);
				m_BloomPipeline->GetShader()->SetFloat("u_Uniforms.LOD", LOD);
				m_BloomPipeline->BindWriteOnlyImage(0, m_BloomTextures[1], i);
				m_BloomTextures[0]->Bind(1);
				m_BloomPipeline->GetShader()->SetTexture("u_BloomTexture", 2, inputTexture);
				m_BloomPipeline->Execute(workGroupsX, workGroupsY, 1, false);

				LOD = i;
				m_BloomPipeline->Begin();
				m_BloomPipeline->GetShader()->SetFloat4("u_Uniforms.Params", params);
				m_BloomPipeline->GetShader()->SetInt("u_Uniforms.Mode", mode);
				m_BloomPipeline->GetShader()->SetFloat("u_Uniforms.LOD", LOD);
				m_BloomPipeline->BindWriteOnlyImage(0, m_BloomTextures[0], i);
				m_BloomTextures[1]->Bind(1);
				m_BloomPipeline->GetShader()->SetTexture("u_BloomTexture", 2, inputTexture);
				m_BloomPipeline->Execute(workGroupsX, workGroupsY, 1, false);
			}
		}

		// Upsample (1)
		{
			if (mipCount < 0)
				return;

			mode = 2;
			LOD--;
			auto [mipWidth, mipHeight] = m_BloomTextures[2]->GetMipSize(mipCount - 2);
			workGroupsX *= 2;
			workGroupsY *= 2;
			workGroupsX = (uint32_t)glm::ceil((float)mipWidth / (float)workGroupSize);
			workGroupsY = (uint32_t)glm::ceil((float)mipHeight / (float)workGroupSize);

			m_BloomPipeline->Begin();
			m_BloomPipeline->GetShader()->SetFloat4("u_Uniforms.Params", params);
			m_BloomPipeline->GetShader()->SetInt("u_Uniforms.Mode", mode);
			m_BloomPipeline->GetShader()->SetFloat("u_Uniforms.LOD", LOD);
			m_BloomPipeline->BindWriteOnlyImage(0, m_BloomTextures[2], mipCount - 2);
			m_BloomTextures[0]->Bind(1);
			m_BloomPipeline->GetShader()->SetTexture("u_BloomTexture", 2, inputTexture);
			m_BloomPipeline->Execute(workGroupsX, workGroupsY, 1, false);
		}

		// Upsample
		{
			mode = 3;
			for (int mip = mipCount - 3; mip >= 0; mip--)
			{
				LOD = mip;

				auto [mipWidth, mipHeight] = m_BloomTextures[2]->GetMipSize(mip);
				workGroupsX = (uint32_t)glm::ceil((float)mipWidth / (float)workGroupSize);
				workGroupsY = (uint32_t)glm::ceil((float)mipHeight / (float)workGroupSize);

				m_BloomPipeline->Begin();
				m_BloomPipeline->GetShader()->SetFloat4("u_Uniforms.Params", params);
				m_BloomPipeline->GetShader()->SetInt("u_Uniforms.Mode", mode);
				m_BloomPipeline->GetShader()->SetFloat("u_Uniforms.LOD", LOD);
				m_BloomPipeline->BindWriteOnlyImage(0, m_BloomTextures[2], mip);
				m_BloomTextures[0]->Bind(1);
				m_BloomTextures[2]->Bind(2);
				m_BloomPipeline->Execute(workGroupsX, workGroupsY, 1, false);
			}
		}

		// Debug
		m_BloomDebugMaterial->SetFloat("u_Settings.Layer", (float)m_Options.BloomDebugMip);
		m_BloomDebugMaterial->SetTexture("u_Texture", 0, m_BloomTextures[m_Options.BloomDebugTex]->GetRendererID());
		Renderer::RenderFullscreenQuad(m_BloomDebugPipeline, m_BloomDebugMaterial);
	}

	void SceneRenderer::CompositePass()
	{
		uint32_t texture;
		if (GetOptions().FXAA)
			texture = m_FXAAPipeline->GetFramebuffer()->GetColorAttachmentRendererID();
		else
			texture = m_GeometryPipeline->GetFramebuffer()->GetColorAttachmentRendererID();

		Ref<Texture2D> bloomDirtMask = Renderer::GetDefaultTexture();
		bool isDirtMaskValid = AssetManager::IsAssetHandleValid(GetOptions().BloomDirtMask);
		bloomDirtMask = isDirtMaskValid ? AssetManager::GetAsset<Texture2D>(GetOptions().BloomDirtMask) : bloomDirtMask;

		//---
		float exposure = GetOptions().Exposure;
		bool useACESTone = GetOptions().ACESTone;
		bool gamaCorrection = GetOptions().GammaCorrection;
		bool bloom = GetOptions().Bloom;
		float bloomIntensity = GetOptions().BloomIntensity;
		float bloomDirtMaskIntensity = GetOptions().BloomDirtMaskIntensity;
		bool grayscale = GetOptions().Grayscale;
		if (m_IsRuntime && !m_RuntimeCamera->UseRendererSettings)
		{
			exposure = m_RuntimeCamera->Exposure;
			useACESTone = m_RuntimeCamera->ACESTone;
			gamaCorrection = m_RuntimeCamera->GammaCorrection;
			bloom = m_RuntimeCamera->Bloom;
			bloomIntensity = m_RuntimeCamera->BloomIntensity;
			bloomDirtMaskIntensity = m_RuntimeCamera->BloomDirtMaskIntensity;
			grayscale = m_RuntimeCamera->Grayscale;

			bloomDirtMask = Renderer::GetDefaultTexture();
			isDirtMaskValid = AssetManager::IsAssetHandleValid(m_RuntimeCamera->BloomDirtMask);
			bloomDirtMask = isDirtMaskValid ? AssetManager::GetAsset<Texture2D>(m_RuntimeCamera->BloomDirtMask) : bloomDirtMask;
		}

		m_CompositeMaterial->SetTexture("u_Texture", 0, texture);

		m_CompositeMaterial->SetFloat("u_Settings.Exposure", exposure);
		m_CompositeMaterial->SetInt("u_Settings.Grayscale", grayscale);
		m_CompositeMaterial->SetInt("u_Settings.ACESTone", useACESTone);
		m_CompositeMaterial->SetInt("u_Settings.GammaCorrection", gamaCorrection);

		m_CompositeMaterial->SetInt("u_Settings.Bloom", bloom);
		m_CompositeMaterial->SetTexture("u_BloomTexture", 1, m_BloomTextures[2]->GetRendererID());
		m_CompositeMaterial->SetTexture("u_BloomDirtMaskTexture", 2, bloomDirtMask->GetRendererID());
		m_CompositeMaterial->SetFloat("u_Settings.BloomIntensity", bloomIntensity);
		m_CompositeMaterial->SetFloat("u_Settings.BloomDirkMaskIntensity", bloomDirtMaskIntensity);

		Renderer::RenderFullscreenQuad(m_CompositePipeline, m_CompositeMaterial);
	}

	void SceneRenderer::Render2DPass()
	{
		for (auto& cmd : m_QuadDrawList)
		{
			if (cmd.Texture)
				Renderer2D::DrawQuad(cmd.Transform, cmd.Texture, cmd.TilingFactor, cmd.TintColor, cmd.ID);
			else
				Renderer2D::DrawQuad(cmd.Transform, cmd.TintColor, cmd.ID);
		}

		for (auto& cmd : m_CircleDrawList)
		{
			Renderer2D::DrawCircle(cmd.Transform, cmd.Color, cmd.Thickness, cmd.Fade, cmd.ID);
		}

		for (auto& cmd : m_RectDrawList)
		{
			Renderer2D::DrawRect(cmd.Transform, cmd.Color, cmd.ID);
		}

		for (auto& cmd : m_BillboardDrawList)
		{
			Renderer2D::DrawQuadBillboard(cmd.Position, cmd.Size, cmd.Texture, cmd.TilingFactor, cmd.TintColor);
		}
	}

	void SceneRenderer::OnImGuiRender(bool& show)
	{
		if (!show)
			return;

		SceneRendererOptions& options = GetOptions();
		auto& window = Application::Get().GetWindow();

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::Begin(ICON_FA_SLIDERS " Renderer Properties", &show);

		if (ImGui::CollapsingHeader("Geral"))
		{
			auto vsync = window.IsVSync();
			if(UI::Checkbox("Vsync", &vsync, true))
				window.SetVSync(vsync);

			UI::Checkbox("Show Grid", &options.ShowGrid, true);
			UI::ColorEdit4("Editor Background Color", m_EditorBackgroundColor);
		}

		if (ImGui::CollapsingHeader("Color and Lightning"))
		{
			UI::DragFloat("Exposure", &options.Exposure, 0.01f, 0.1f, 100.0f, true);
			UI::Checkbox("Grayscale", &options.Grayscale, true);
			UI::Checkbox("ACES Tone Mapping", &options.ACESTone, true);
			UI::Checkbox("Gamma Correction", &options.GammaCorrection);
		}

		if (ImGui::CollapsingHeader("Shadows"))
		{
			UI::Checkbox("Soft Shadows", &m_RendererBuffer.SoftShadows, true);
			UI::Checkbox("Cascade Fading", &m_RendererBuffer.CascadeFading, true);
			
			UI::DragFloat("Max. Shadow Distance", &m_RendererBuffer.MaxShadowDistance, 0.1f, 0.0f, 1000000.0f, true);
			UI::DragFloat("Shadow Fade", &m_RendererBuffer.ShadowFade, 0.1f, 0.0f, 1000000.0f, true);
			UI::DragFloat("Cascade Transition Fade", &m_RendererBuffer.CascadeTransitionFade, 0.1f, 0.0f, 1000000.0f, true);
			UI::DragFloat("Cascade Split", &options.CascadeSplitLambda, 0.1f, 0.0f, 1000000.0f, true);
			UI::DragFloat("Cascade Near Plane Offeset", &options.CascadeNearPlaneOffset, 0.1f, 0.0f, 1000000.0f, true);
			UI::DragFloat("Cascade Far Plane Offeset", &options.CascadeFarPlaneOffset, 0.1f, 0.0f, 1000000.0f, true);
			UI::DragFloat("Dir. Light Size", &m_RendererBuffer.LightSize, 0.1f, 0.0f, 1000000.0f, true);

			UI::Checkbox("Show Cascade in Viewport", &m_RendererBuffer.ShowCascades, true);

			UI::SetPosX(ImGui::GetContentRegionMax().x - 70);
			if (ImGui::Button("Reset", ImVec2{ 70, 30 }))
			{
				m_RendererBuffer = RendererData();
				options.CascadeNearPlaneOffset = -50.0f;
				options.CascadeFarPlaneOffset = 50.0f;
				options.CascadeSplitLambda = 0.92f;
			}

			UI::ShiftPos(20.0f, 10.0f);
			ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			if(ImGui::TreeNodeEx("Shadow Map", treeNodeFlags))
			{
				uint32_t shadowMapRendererID = m_TempPipeline->GetFramebuffer()->GetColorAttachmentRendererID();
				UI::SliderInt("Cascade", &options.ShadowMapDebugCascade, 0, 3);
				UI::ShiftPosY(5.0f);
				ImGui::Image(reinterpret_cast<void*>(shadowMapRendererID), ImVec2{ 256, 256 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

				ImGui::TreePop();
			}
			
		}

		if (ImGui::CollapsingHeader("Bloom"))
		{
			UI::Checkbox("Enable", &options.Bloom, true);
			UI::DragFloat("Intensity", &options.BloomIntensity, 0.1f, 0.0f, 10000.0f, true);
			UI::DragFloat("Threshold", &options.BloomThreshold, 0.1f, 0.0f, 100.0f, true);
			UI::DragFloat("Knee", &options.BloomKnee, 0.1f, 0.0f, 100.0f, true);

			Ref<Texture2D> dirtMask = Renderer::GetDefaultTexture();
			bool isMaskValid = AssetManager::IsAssetHandleValid(options.BloomDirtMask);
			dirtMask = isMaskValid ? AssetManager::GetAsset<Texture2D>(options.BloomDirtMask) : dirtMask;
			ImGui::Columns(2);
			ImGui::Text("Dirt Mask");
			ImGui::SameLine();
			ImGui::NextColumn();
			ImGui::Image(reinterpret_cast<void*>(dirtMask->GetRendererID()), { 64, 64 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_TEXTURE"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;

					AssetHandle handle = AssetManager::GetHandle(path);
					options.BloomDirtMask = handle;
				}

				ImGui::EndDragDropTarget();
			}
			if (isMaskValid)
			{
				ImGui::SameLine();
				if (ImGui::Button(ICON_FA_WINDOW_CLOSE, ImVec2(18, 18)))
				{
					options.BloomDirtMask = 0;
				}
			}
			ImGui::Columns(1);
			ImGui::Separator();

			UI::DragFloat("Dirt Mask Intensity", &options.BloomDirtMaskIntensity, 0.1f, 0.0f, 1000.0f, true);

			UI::SetPosX(ImGui::GetContentRegionMax().x - 70);
			if (ImGui::Button("Reset", ImVec2{ 70, 30 }))
			{
				options.BloomIntensity = 1.0f;
				options.BloomThreshold = 1.0f;
				options.BloomKnee = 0.1f;
				options.BloomDirtMask = 0;
				options.BloomDirtMaskIntensity = 1.0f;
			}

			UI::ShiftPos(20.0f, 10.0f);
			ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			if (ImGui::TreeNodeEx("Bloom Textures", treeNodeFlags))
			{
				UI::SliderInt("Bloom Texture", &options.BloomDebugTex, 0, 2);
				uint32_t bloomDebugTex = m_BloomDebugPipeline->GetFramebuffer()->GetColorAttachmentRendererID();
				uint32_t maxMipLevel = m_BloomTextures[options.BloomDebugTex]->GetMipLevelCount();
				UI::SliderInt("Bloom Mipmap Level", &options.BloomDebugMip, 0, maxMipLevel - 1);
				UI::ShiftPosY(5.0f);
				ImGui::Image(reinterpret_cast<void*>(bloomDebugTex), ImVec2{ 256, 256 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
				Renderer::SetDebugTexture(bloomDebugTex);
				ImGui::TreePop();
			}
		}

		if (ImGui::CollapsingHeader("Anti-Aliasing"))
		{
			UI::Checkbox("FXAA", &options.FXAA, true);

			UI::ShiftPos(20.0f, 10.0f);
			ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			if (ImGui::TreeNodeEx("Advanced", treeNodeFlags))
			{
				UI::DragFloat("Threshold Min", &options.FXAAThresholdMin, 1.0f, 1.0f, 256.0f, false);
				UI::DragFloat("Threshold Max", &options.FXAAThresholdMax, 1.0f, 1.0f, 256.0f, false);
				UI::SliderInt("Iterations", &options.FXAAIterations, 1, 48, false);
				UI::DragFloat("Sub-pixel Quality", &options.FXAASubPixelQuality, 0.1f, 0.1f, 10.0f, false);

				ImGui::TreePop();
			}
		}

		ImGui::End();
	}

	void SceneRenderer::CalculateCascades(CascadeData* cascades, const CameraInfo& camera, const glm::vec3& lightDirection)
	{
		SceneRendererOptions options = GetOptions();

		auto viewProjection = camera.Camera.GetProjectionMatrix() * camera.ViewMatrix;

		const int SHADOW_MAP_CASCADE_COUNT = 4;
		float cascadeSplits[SHADOW_MAP_CASCADE_COUNT];

		float nearClip = camera.Near;
		float farClip = camera.Far;
		float clipRange = farClip - nearClip;

		float minZ = nearClip;
		float maxZ = nearClip + clipRange;

		float range = maxZ - minZ;
		float ratio = maxZ / minZ;

		// Calculate split depths based on view camera frustum
		// Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
		for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
		{
			float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
			float log = minZ * std::pow(ratio, p);
			float uniform = minZ + range * p;
			float d = options.CascadeSplitLambda * (log - uniform) + uniform;
			cascadeSplits[i] = (d - nearClip) / clipRange;
		}

		cascadeSplits[3] = 0.3f;

		// Manually set cascades here
		// cascadeSplits[0] = 0.05f;
		// cascadeSplits[1] = 0.15f;
		// cascadeSplits[2] = 0.3f;
		// cascadeSplits[3] = 1.0f;

		// Calculate orthographic projection matrix for each cascade
		float lastSplitDist = 0.0;
		for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
		{
			float splitDist = cascadeSplits[i];

			glm::vec3 frustumCorners[8] =
			{
				glm::vec3(-1.0f,  1.0f, -1.0f),
				glm::vec3(1.0f,  1.0f, -1.0f),
				glm::vec3(1.0f, -1.0f, -1.0f),
				glm::vec3(-1.0f, -1.0f, -1.0f),
				glm::vec3(-1.0f,  1.0f,  1.0f),
				glm::vec3(1.0f,  1.0f,  1.0f),
				glm::vec3(1.0f, -1.0f,  1.0f),
				glm::vec3(-1.0f, -1.0f,  1.0f),
			};

			// Project frustum corners into world space
			glm::mat4 invCam = glm::inverse(viewProjection);
			for (uint32_t i = 0; i < 8; i++)
			{
				glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
				frustumCorners[i] = invCorner / invCorner.w;
			}

			for (uint32_t i = 0; i < 4; i++)
			{
				glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
				frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
				frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
			}

			// Get frustum center
			glm::vec3 frustumCenter = glm::vec3(0.0f);
			for (uint32_t i = 0; i < 8; i++)
				frustumCenter += frustumCorners[i];

			frustumCenter /= 8.0f;

			//frustumCenter *= 0.01f;

			float radius = 0.0f;
			for (uint32_t i = 0; i < 8; i++)
			{
				float distance = glm::length(frustumCorners[i] - frustumCenter);
				radius = glm::max(radius, distance);
			}
			radius = std::ceil(radius * 16.0f) / 16.0f;

			glm::vec3 maxExtents = glm::vec3(radius);
			glm::vec3 minExtents = -maxExtents;

			glm::vec3 lightDir = -lightDirection;
			glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f + options.CascadeNearPlaneOffset, maxExtents.z - minExtents.z + options.CascadeFarPlaneOffset);

			// Offset to texel space to avoid shimmering (from https://stackoverflow.com/questions/33499053/cascaded-shadow-map-shimmering)
			glm::mat4 shadowMatrix = lightOrthoMatrix * lightViewMatrix;
			float ShadowMapResolution = m_ShadowPipeline->GetFramebuffer()->GetSpecification().Width;
			glm::vec4 shadowOrigin = (shadowMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) * ShadowMapResolution / 2.0f;
			glm::vec4 roundedOrigin = glm::round(shadowOrigin);
			glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
			roundOffset = roundOffset * 2.0f / ShadowMapResolution;
			roundOffset.z = 0.0f;
			roundOffset.w = 0.0f;

			lightOrthoMatrix[3] += roundOffset;

			// Store split distance and matrix in cascade
			cascades[i].SplitDepth = (nearClip + splitDist * clipRange) * -1.0f;
			cascades[i].ViewProj = lightOrthoMatrix * lightViewMatrix;
			cascades[i].View = lightViewMatrix;

			lastSplitDist = cascadeSplits[i];
		}
	}

	Ref<Framebuffer> SceneRenderer::GetGeometryBuffer()
	{
		return m_GeometryPipeline->GetFramebuffer();
	}

	Ref<Framebuffer> SceneRenderer::GetFinalBuffer()
	{
		return m_CompositePipeline->GetFramebuffer();
	}

	uint32_t SceneRenderer::GetFinalImage()
	{
		return m_CompositePipeline->GetFramebuffer()->GetColorAttachmentRendererID();
	}
}