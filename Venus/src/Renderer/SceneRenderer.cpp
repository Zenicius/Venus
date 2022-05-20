#include "pch.h"
#include "SceneRenderer.h"

#include "Renderer.h"

namespace Venus {

	SceneRenderer::SceneRenderer(Ref<Scene> scene)
		:m_Scene(scene)
	{
		Init();
	}

	void SceneRenderer::Init()
	{
		// Geometry 
		{
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::DEPTH24STENCIL8 };
			fbSpec.Samples = 1;
			fbSpec.SwapChainTarget = false;
			fbSpec.Width = 1920;
			fbSpec.Height = 1080;
			Ref<Framebuffer> framebuffer = Framebuffer::Create(fbSpec);

			PipelineSpecification pipelineSpec;
			pipelineSpec.Shader = Shader::Create("assets/shaders/PBR.glsl");
			pipelineSpec.Framebuffer = framebuffer;

			m_GeometryPipeline = CreateRef<Pipeline>(pipelineSpec);
		}

		// Selected Geometry
		{
			FramebufferSpecification fbSpec;
			fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::DEPTH24STENCIL8 };
			fbSpec.Samples = 1;
			fbSpec.SwapChainTarget = false;
			fbSpec.Width = 1920;
			fbSpec.Height = 1080;
			Ref<Framebuffer> framebuffer = Framebuffer::Create(fbSpec);

			PipelineSpecification pipelineSpec;
			pipelineSpec.Shader = Shader::Create("assets/shaders/Outline.glsl");
			pipelineSpec.Framebuffer = framebuffer;

			m_SelectedGeometryPipeline = CreateRef<Pipeline>(pipelineSpec);
		}

		// Grid
		{
			PipelineSpecification pipelineSpec;
			pipelineSpec.Shader = Shader::Create("assets/shaders/Grid.glsl");
			pipelineSpec.Framebuffer = m_GeometryPipeline->GetFramebuffer();

			m_GridPipeline = CreateRef<Pipeline>(pipelineSpec);
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
			//pipelineSpec.Shader = Shader::Create("");
			pipelineSpec.Framebuffer = framebuffer;

			m_CompositePipeline = CreateRef<Pipeline>(pipelineSpec);
		}
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

	void SceneRenderer::BeginScene(Camera& camera, const glm::mat4& transform)
	{
		if (m_NeedsResize)
		{
			m_GeometryPipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
			m_SelectedGeometryPipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
			m_CompositePipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
		}

		Renderer::Clear(m_GeometryPipeline->GetFramebuffer());
		Renderer::Clear(m_SelectedGeometryPipeline->GetFramebuffer());
		Renderer::Clear(m_CompositePipeline->GetFramebuffer());

		Renderer::ResetStats(); Renderer::ResetStats();

		Renderer::BeginScene(camera, transform);
	}

	void SceneRenderer::BeginScene(EditorCamera& camera)
	{
		if (m_NeedsResize)
		{
			m_GeometryPipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
			m_SelectedGeometryPipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
			m_CompositePipeline->GetFramebuffer()->Resize(m_ViewportWidth, m_ViewportHeight);
		}

		Renderer::Clear(m_GeometryPipeline->GetFramebuffer());
		Renderer::Clear(m_SelectedGeometryPipeline->GetFramebuffer());
		Renderer::Clear(m_CompositePipeline->GetFramebuffer());

		Renderer::ResetStats();

		Renderer::BeginScene(camera);
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

	void SceneRenderer::SubmitModel(Ref<Model> model, const glm::mat4& transform)
	{
		DrawCmd drawCmd;
		drawCmd.Model = model;
		drawCmd.Transform = transform;

		m_DrawList.push_back(drawCmd);
	}

	void SceneRenderer::SubmitSelectedModel(Ref<Model> model, const glm::mat4& transform)
	{
		DrawCmd drawCmd;
		drawCmd.Model = model;
		drawCmd.Transform = transform;

		m_DrawList.push_back(drawCmd);
		m_SelectedDrawList.push_back(drawCmd);
	}

	void SceneRenderer::Flush()
	{
		GeometryPass();
		CompositePass();

		m_DrawList.clear();
		m_SelectedDrawList.clear();
	}

	void SceneRenderer::GeometryPass()
	{
		for (auto& cmd : m_DrawList)
		{
			Renderer::RenderModel(m_GeometryPipeline, cmd.Model, cmd.Transform);
		}

		const glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(8.0f));
		Renderer::RenderQuad(m_GridPipeline, transform);
	}

	void Venus::SceneRenderer::CompositePass()
	{

	}

	Ref<Framebuffer> SceneRenderer::GetFinalRender()
	{
		return m_CompositePipeline->GetFramebuffer();
	}

	uint32_t SceneRenderer::GetFinalImage()
	{
		return m_GeometryPipeline->GetFramebuffer()->GetColorAttachmentRendererID();
	}
}