#include "pch.h"
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/UniformBuffer.h"

namespace Venus {

	struct RendererData
	{
		// Camera Data
		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

		// Model Data
		struct ModelData
		{
			glm::mat4 Transform;
			int EntityID;
		};
		ModelData ModelBuffer;
		Ref<UniformBuffer> ModelDataBuffer;

		// Shaders
		Ref<Shader> PBRShader;

		// Stats
		Renderer::Statistics Stats;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		VS_PROFILE_FUNCTION();

		s_Data.PBRShader = Shader::Create("assets/shaders/PBR.glsl");
	
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(RendererData::CameraData), 0);
		s_Data.ModelDataBuffer = UniformBuffer::Create(sizeof(RendererData::ModelData), 1);

		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		VS_PROFILE_FUNCTION();

		Renderer2D::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		VS_PROFILE_FUNCTION();

		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		VS_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));

		Renderer2D::BeginScene();
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		VS_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));

		Renderer2D::BeginScene();
	}

	void Renderer::EndScene()
	{
		Renderer2D::EndScene();
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		VS_PROFILE_FUNCTION();

		s_Data.ModelBuffer.Transform = transform;
		s_Data.ModelBuffer.EntityID = -1;
		s_Data.ModelDataBuffer->SetData(&s_Data.ModelBuffer, sizeof(RendererData::ModelData));

		shader->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::SubmitModel(const Ref<Model>& model, const glm::mat4& transform, int entityID)
	{
		VS_PROFILE_FUNCTION();

		/*
		s_Data.ModelVertexArray->AddVertexBuffer(model.m_VertexBuffer);
		s_Data.ModelVertexArray->SetIndexBuffer(model.m_IndexBuffer);

		s_Data.PBRShader->Bind();
		RenderCommand::DrawIndexed(s_Data.ModelVertexArray);
		*/

		s_Data.ModelBuffer.Transform = transform;
		s_Data.ModelBuffer.EntityID = entityID;
		s_Data.ModelDataBuffer->SetData(&s_Data.ModelBuffer, sizeof(RendererData::ModelData));

		s_Data.PBRShader->Bind();
		
		for (auto& mesh : model->m_Meshes)
		{
			s_Data.Stats.VertexCount += mesh.m_Vertices.size();
			s_Data.Stats.IndexCount += mesh.m_Indices.size();
			s_Data.Stats.DrawCalls++;

			for (auto& texture : mesh.m_Textures)
			{
				if ((int)texture.Type == 0)
				{
					model->m_DiffuseMaps[texture.Index]->Bind(0);
				}
			}
			
			RenderCommand::DrawIndexed(mesh.m_VertexArray);
		}
	}

	void Renderer::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Renderer::Statistics));

		Renderer2D::ResetStats();
	}

	Renderer::Statistics Renderer::GetStats()
	{
		return s_Data.Stats;
	}

}
