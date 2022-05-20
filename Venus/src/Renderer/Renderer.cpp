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
			glm::mat4 ViewMatrix;
			glm::mat4 ProjectionMatrix;
			glm::mat4 ViewProjectionMatrix;
			glm::vec3 Position;
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

		// Quad Data
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<IndexBuffer> QuadIndexBuffer;

		// Default Texture
		Ref<Texture2D> DefaultTexture;

		// Stats
		Renderer::Statistics Stats;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		VS_PROFILE_FUNCTION();

		// Quad
		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoord;
		};
		float x = -1, y = -1, width = 2, height = 2;

		QuadVertex* data = new QuadVertex[4];
		data[0].Position = glm::vec3(x, y, 0.0f);
		data[0].TexCoord = glm::vec2(0.0f, 0.0f);

		data[1].Position = glm::vec3(x + width, y, 0.0f);
		data[1].TexCoord = glm::vec2(1.0f, 0.0f);	

		data[2].Position = glm::vec3(x + width, y + height, 0.0f);
		data[2].TexCoord = glm::vec2(1.0f, 1.0f);
		
		data[3].Position = glm::vec3(x,  y + height, 0.0f);
		data[3].TexCoord = glm::vec2(0.0f, 1.0f);

		s_Data.QuadVertexBuffer = VertexBuffer::Create(data, 4 * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" },
		});
		
		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
		s_Data.QuadIndexBuffer = IndexBuffer::Create(indices, 6 * sizeof(uint32_t));

		s_Data.QuadVertexArray = VertexArray::Create();
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
		s_Data.QuadVertexArray->SetIndexBuffer(s_Data.QuadIndexBuffer);

		// Default white Texture
		s_Data.DefaultTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.DefaultTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		
		// Uniforms
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

	void Renderer::Clear(Ref<Framebuffer> framebuffer, glm::vec4 color)
	{
		framebuffer->Bind();
		
		RenderCommand::SetClearColor(color);
		RenderCommand::Clear();

		framebuffer->Unbind();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		VS_PROFILE_FUNCTION();

		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		VS_PROFILE_FUNCTION();
		
		// TEMP HACK Todo: Send correct values
		s_Data.CameraBuffer.ViewMatrix = glm::mat4(1.0f);
		s_Data.CameraBuffer.ProjectionMatrix = camera.GetProjection();
		s_Data.CameraBuffer.ViewProjectionMatrix = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraBuffer.Position = glm::vec3(1.0f);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));

		Renderer2D::BeginScene();
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		VS_PROFILE_FUNCTION();
		
		s_Data.CameraBuffer.ViewMatrix = camera.GetViewMatrix();
		s_Data.CameraBuffer.ProjectionMatrix = camera.GetProjection();
		s_Data.CameraBuffer.ViewProjectionMatrix = camera.GetViewProjection();
		s_Data.CameraBuffer.Position = camera.GetPosition();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));

		Renderer2D::BeginScene();
	}

	void Renderer::EndScene()
	{
		Renderer2D::EndScene();
	}

	void Renderer::Render(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		VS_PROFILE_FUNCTION();

		s_Data.ModelBuffer.Transform = transform;
		s_Data.ModelBuffer.EntityID = -1;
		s_Data.ModelDataBuffer->SetData(&s_Data.ModelBuffer, sizeof(RendererData::ModelData));

		shader->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::RenderQuad(const Ref<Pipeline>& pipeline, const glm::mat4& transform)
	{
		VS_PROFILE_FUNCTION();

#if 0
		s_Data.GridBuffer.Transform = transform;
		s_Data.GridBuffer.Scale = scale;
		s_Data.GridBuffer.Size = size;
		s_Data.GridDataBuffer->SetData(&s_Data.GridBuffer, sizeof(RendererData::GridData));

		s_Data.GridShader->Bind();

		s_Data.Stats.VertexCount += 4;
		s_Data.Stats.IndexCount += 6;
		s_Data.Stats.DrawCalls++;

		RenderCommand::DrawIndexed(s_Data.GridVertexArray);
#endif

		s_Data.ModelBuffer.Transform = transform;
		s_Data.ModelBuffer.EntityID = -1;
		s_Data.ModelDataBuffer->SetData(&s_Data.ModelBuffer, sizeof(RendererData::ModelData));

		pipeline->GetFramebuffer()->Bind();
		pipeline->GetShader()->Bind();

		s_Data.Stats.VertexCount += 4;
		s_Data.Stats.IndexCount += 6;
		s_Data.Stats.DrawCalls++;

		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);

		pipeline->GetFramebuffer()->Unbind();
	}

	void Renderer::RenderModel(const Ref<Pipeline>& pipeline, const Ref<Model>& model, const glm::mat4& transform, int entityID)
	{
		VS_PROFILE_FUNCTION();

#if 0
		s_Data.ModelBuffer.Transform = transform;
		s_Data.ModelBuffer.EntityID = entityID;
		s_Data.ModelDataBuffer->SetData(&s_Data.ModelBuffer, sizeof(RendererData::ModelData));

		s_Data.PBRShader->Bind();
		
		for (auto& mesh : model->m_Meshes)
		{
			s_Data.Stats.VertexCount += mesh.m_Vertices.size();
			s_Data.Stats.IndexCount += mesh.m_Indices.size();
			s_Data.Stats.DrawCalls++;

			// Temp Hack
			bool diffuseTex = false;
			for (auto& texture : mesh.m_Textures)
			{
				if ((int)texture.Type == 0)
				{
					model->m_DiffuseMaps[texture.Index]->Bind(0);
					diffuseTex = true;
				}
				
				if(!diffuseTex)
					s_Data.DefaultTexture->Bind(0);
			}
			
			RenderCommand::DrawIndexed(mesh.m_VertexArray);
		}
#endif

		pipeline->GetFramebuffer()->Bind();
		pipeline->GetShader()->Bind();

		s_Data.ModelBuffer.Transform = transform;
		s_Data.ModelBuffer.EntityID = entityID;
		s_Data.ModelDataBuffer->SetData(&s_Data.ModelBuffer, sizeof(RendererData::ModelData));


		for (auto& mesh : model->m_Meshes)
		{
			s_Data.Stats.VertexCount += mesh.m_Vertices.size();
			s_Data.Stats.IndexCount += mesh.m_Indices.size();
			s_Data.Stats.DrawCalls++;

			// Temp Hack
			bool diffuseTex = false;
			for (auto& texture : mesh.m_Textures)
			{
				if ((int)texture.Type == 0)
				{
					model->m_DiffuseMaps[texture.Index]->Bind(0);
					diffuseTex = true;
				}

				if (!diffuseTex)
					s_Data.DefaultTexture->Bind(0);
			}

			RenderCommand::DrawIndexed(mesh.m_VertexArray);
		}

		pipeline->GetFramebuffer()->Unbind();
	}

	void Renderer::RenderSelectedModel(const Ref<Pipeline>& pipeline, const Ref<Model>& model, const glm::mat4& transform, int entityID)
	{
		VS_PROFILE_FUNCTION();

#if 0
		// First Pass, Normal Model
		RenderCommand::EnableStencilTest();
		RenderCommand::SetStencilTest(0x0207, 1, 0xFF); // GL_ALWAYS
		RenderCommand::EnableStencilWrite();
		RenderModel(model, transform, entityID);

		// Second Pass, Outlined
		s_Data.BlurFramebuffer->Bind();

		RenderCommand::DisableDepthTest();
		RenderCommand::DisableStencilWrite();
	
		s_Data.ModelBuffer.Transform = transform;
		s_Data.ModelBuffer.EntityID = entityID;
		s_Data.ModelDataBuffer->SetData(&s_Data.ModelBuffer, sizeof(RendererData::ModelData));

		s_Data.OutlineShader->Bind();

		for (auto& mesh : model->m_Meshes)
		{
			s_Data.Stats.VertexCount += mesh.m_Vertices.size();
			s_Data.Stats.IndexCount += mesh.m_Indices.size();
			s_Data.Stats.DrawCalls++;

			s_Data.DefaultTexture->Bind(0);

			RenderCommand::DrawIndexed(mesh.m_VertexArray);
		}

		// Third Pass, Blur
		s_Data.TargetFramebuffer->Bind();

		RenderCommand::DisableDepthTest();
		RenderCommand::EnableStencilTest();
		RenderCommand::SetStencilTest(0x0205, 1, 0xFF); // GL_NOTEQUAL
		RenderCommand::DisableStencilWrite();

		s_Data.BlurShader->Bind();

		s_Data.Stats.VertexCount += 4;
		s_Data.Stats.IndexCount += 6;
		s_Data.Stats.DrawCalls++;

		RenderCommand::BindTexture(s_Data.BlurFramebuffer->GetColorAttachmentRendererID());
		RenderCommand::DrawIndexed(s_Data.GridVertexArray);

		// Reset
		RenderCommand::EnableStencilWrite();
		RenderCommand::DisableStencilTest();
		RenderCommand::EnableDepthTest();

		// First Pass, Normal Model
		RenderCommand::EnableStencilTest();
		RenderCommand::SetStencilTest(0x0207, 1, 0xFF); // GL_ALWAYS
		RenderCommand::EnableStencilWrite();
		RenderModel(model, transform, entityID);

		// Second Pass, Outlined
		RenderCommand::DisableDepthTest();
		RenderCommand::DisableStencilWrite();
		RenderCommand::SetStencilTest(0x0205, 1, 0xFF); // GL_NOTEQUAL

		s_Data.ModelBuffer.Transform = transform;
		s_Data.ModelBuffer.EntityID = entityID;
		s_Data.ModelDataBuffer->SetData(&s_Data.ModelBuffer, sizeof(RendererData::ModelData));

		s_Data.OutlineShader->Bind();

		for (auto& mesh : model->m_Meshes)
		{
			s_Data.Stats.VertexCount += mesh.m_Vertices.size();
			s_Data.Stats.IndexCount += mesh.m_Indices.size();
			s_Data.Stats.DrawCalls++;

			s_Data.DefaultTexture->Bind(0);

			RenderCommand::DrawIndexed(mesh.m_VertexArray);
		}

		// Reset
		RenderCommand::EnableStencilWrite();
		RenderCommand::DisableStencilTest();
		RenderCommand::SetStencilTest(0x0207, 0, 0xFF); // GL_ALWAYS
		RenderCommand::EnableDepthTest();
#endif
	}

	void Renderer::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Renderer::Statistics));

		Renderer2D::ResetStats();
	}

	Ref<Texture2D> Renderer::GetDefaultTexture()
	{
		return s_Data.DefaultTexture;
	}

	Renderer::Statistics Renderer::GetStats()
	{
		return s_Data.Stats;
	}

}
