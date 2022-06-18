#include "pch.h"
#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/UniformBuffer.h"
#include "Renderer/ComputePipeline.h"

#include "glad/glad.h"

#include "imgui.h"

namespace Venus {

	struct RendererData
	{
		//-- Shaders--------------------------------
		Ref<ShaderLibrary> ShaderLibrary;
		//------------------------------------------


		//-- Camera Data----------------------------
		struct CameraData
		{
			glm::mat4 ViewMatrix;
			glm::mat4 ProjectionMatrix;
			glm::mat4 ViewProjectionMatrix;
			glm::mat4 InverseViewProjectionMatrix;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
		//------------------------------------------


		//-- Model Data-----------------------------
		struct ModelData
		{
			glm::mat4 Transform;
			int EntityID;
		};
		ModelData ModelBuffer;
		Ref<UniformBuffer> ModelDataBuffer;
		//------------------------------------------


		//-- Quad Data------------------------------
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<IndexBuffer> QuadIndexBuffer;
		//------------------------------------------


		//-- Cube Data------------------------------
		Ref<VertexArray> CubeVertexArray;
		Ref<VertexBuffer> CubeVertexBuffer;
		Ref<IndexBuffer> CubeIndexBuffer;
		//------------------------------------------
		

		//-- Default Textures-----------------------
		Ref<Texture2D> DefaultTexture;
		Ref<TextureCube> DefaultCube;
		Ref<Texture2D> BRDFLutTexture;
		uint32_t DebugTexture;
		float DebugParam;
		//------------------------------------------


		//-- Environment----------------------------
		Ref<SceneEnvironment> EnviromentMap;
		Ref<Material> EnvironmentMaterial;
		uint32_t ShadowMap;
		//------------------------------------------


		//-- Stats----------------------------------
		Renderer::Statistics Stats;
		//------------------------------------------
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		VS_PROFILE_FUNCTION();

		//-- Shaders-----------------------------------------------------------------------------------
		s_Data.ShaderLibrary = ShaderLibrary::Create();
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/PBR.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/Outline.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/Grid.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/Skybox.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/ShadowMap.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/ShadowMapDebug.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/FXAA.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/Bloom.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/BloomDebug.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/Composite.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/EquirectangularToCubeMap.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/EnvironmentMipFilter.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/EnvironmentIrradiance.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/PreethamSky.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/Renderer2D_Quad.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/Renderer2D_Circle.glsl");
		Renderer::GetShaderLibrary()->Load("Resources/Shaders/Renderer2D_Line.glsl");
		//---------------------------------------------------------------------------------------------


		//-- Quad Data---------------------------------------------------------------------------------
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
		s_Data.QuadIndexBuffer = IndexBuffer::Create(indices, 6);

		s_Data.QuadVertexArray = VertexArray::Create();
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);
		s_Data.QuadVertexArray->SetIndexBuffer(s_Data.QuadIndexBuffer);
		//---------------------------------------------------------------------------------------------


		//-- Cube Data---------------------------------------------------------------------------------
		float CubeVertices[] =
		{
			-1.0f, -1.0f,  1.0f,  //        7--------6
			 1.0f, -1.0f,  1.0f,  //       /|       /|
			 1.0f, -1.0f, -1.0f,  //      4--------5 |
			-1.0f, -1.0f, -1.0f,  //      | |      | |
			-1.0f,  1.0f,  1.0f,  //      | 3------|-2
			 1.0f,  1.0f,  1.0f,  //      |/       |/
			 1.0f,  1.0f, -1.0f,  //      0--------1
			-1.0f,  1.0f, -1.0f
		};

		uint32_t CubeIndices[] =
		{
			// Right
			1, 2, 6,
			6, 5, 1,
			// Left
			0, 4, 7,
			7, 3, 0,
			// Top
			4, 5, 6,
			6, 7, 4,
			// Bottom
			0, 3, 2,
			2, 1, 0,
			// Back
			0, 1, 5,
			5, 4, 0,
			// Front
			3, 7, 6,
			6, 2, 3
		};

		s_Data.CubeVertexBuffer = VertexBuffer::Create(CubeVertices, sizeof(float) * 24);
		s_Data.CubeIndexBuffer = IndexBuffer::Create(CubeIndices, 36);
		s_Data.CubeVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" }
		});

		s_Data.CubeVertexArray = VertexArray::Create();
		s_Data.CubeVertexArray->AddVertexBuffer(s_Data.CubeVertexBuffer);
		s_Data.CubeVertexArray->SetIndexBuffer(s_Data.CubeIndexBuffer);
		//---------------------------------------------------------------------------------------------


		//-- Default Textures--------------------------------------------------------------------------
		s_Data.DefaultTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.DefaultTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		s_Data.DefaultCube = TextureCube::Create(1, 1);
		uint32_t blackTextureData = 0xff000000;
		s_Data.DefaultCube->SetData(&blackTextureData, sizeof(uint32_t));

		TextureProperties props;
		props.WrapMode = TextureWrapMode::ClampToBorder;
		s_Data.BRDFLutTexture = Texture2D::Create("Resources/Textures/BRDF_LUT.tga", props);

		s_Data.DebugTexture = 0;
		s_Data.DebugParam = 0.0f;
		//---------------------------------------------------------------------------------------------


		//-- Environment-------------------------------------------------------------------------------
		s_Data.EnvironmentMaterial = Material::Create(Renderer::GetShaderLibrary()->Get("PBR"));
		s_Data.EnviromentMap = Renderer::CreateEmptyEnvironmentMap();
		s_Data.ShadowMap = 0;
		//---------------------------------------------------------------------------------------------
		

		//-- Uniforms----------------------------------------------------------------------------------
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(RendererData::CameraData), 0);
		s_Data.ModelDataBuffer = UniformBuffer::Create(sizeof(RendererData::ModelData), 1);
		//---------------------------------------------------------------------------------------------

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
		
		s_Data.CameraBuffer.ViewMatrix = glm::inverse(transform);
		s_Data.CameraBuffer.ProjectionMatrix = camera.GetProjectionMatrix();
		s_Data.CameraBuffer.ViewProjectionMatrix = camera.GetProjectionMatrix() * glm::inverse(transform);
		s_Data.CameraBuffer.InverseViewProjectionMatrix = glm::inverse(camera.GetProjectionMatrix() * glm::inverse(transform));
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));

		Renderer2D::BeginScene();
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		VS_PROFILE_FUNCTION();
		
		s_Data.CameraBuffer.ViewMatrix = camera.GetViewMatrix();
		s_Data.CameraBuffer.ProjectionMatrix = camera.GetProjectionMatrix();
		s_Data.CameraBuffer.ViewProjectionMatrix = camera.GetViewProjection();
		s_Data.CameraBuffer.InverseViewProjectionMatrix = glm::inverse(camera.GetViewProjection());
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

		pipeline->GetFramebuffer()->Bind();
		pipeline->GetShader()->Bind();

		s_Data.ModelBuffer.Transform = transform;
		s_Data.ModelBuffer.EntityID = -1;
		s_Data.ModelDataBuffer->SetData(&s_Data.ModelBuffer, sizeof(RendererData::ModelData));

		s_Data.Stats.VertexCount += 4;
		s_Data.Stats.IndexCount += 6;
		s_Data.Stats.DrawCalls++;

		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);

		pipeline->GetFramebuffer()->Unbind();
	}

	void Renderer::RenderQuadWithMaterial(const Ref<Pipeline>& pipeline, const glm::mat4& transform, const Ref<Material>& material)
	{
		VS_PROFILE_FUNCTION();

		pipeline->GetFramebuffer()->Bind();
		pipeline->GetShader()->Bind();
		material->Bind();

		s_Data.ModelBuffer.Transform = transform;
		s_Data.ModelBuffer.EntityID = -1;
		s_Data.ModelDataBuffer->SetData(&s_Data.ModelBuffer, sizeof(RendererData::ModelData));

		s_Data.Stats.VertexCount += 4;
		s_Data.Stats.IndexCount += 6;
		s_Data.Stats.DrawCalls++;

		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);

		pipeline->GetFramebuffer()->Unbind();
	}

	void Renderer::RenderFullscreenQuad(const Ref<Pipeline>& pipeline, const Ref<Material>& material)
	{
		pipeline->GetFramebuffer()->Bind();
		pipeline->GetShader()->Bind();
		material->Bind();

		s_Data.Stats.VertexCount += 4;
		s_Data.Stats.IndexCount += 6;
		s_Data.Stats.DrawCalls++;

		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);

		pipeline->GetFramebuffer()->Unbind();
	}

	void Renderer::RenderCube(const Ref<Pipeline>& pipeline, const Ref<Material>& material)
	{
		glDepthMask(GL_FALSE);
		pipeline->GetFramebuffer()->Bind();
		pipeline->GetShader()->Bind();
		material->Bind();

		RenderCommand::DrawIndexed(s_Data.CubeVertexArray);

		pipeline->GetFramebuffer()->Unbind();
		glDepthMask(GL_TRUE);
	}

	void Renderer::RenderModel(const Ref<Pipeline>& pipeline, const Ref<Model>& model, const glm::mat4& transform, int entityID)
	{
		VS_PROFILE_FUNCTION();

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

			RenderCommand::DrawIndexed(mesh.m_VertexArray);
		}

		pipeline->GetFramebuffer()->Unbind();
	}

	void Renderer::RenderModelWithMaterial(const Ref<Pipeline>& pipeline, const Ref<Model>& model, const glm::mat4& transform, int entityID)
	{
		VS_PROFILE_FUNCTION();

		pipeline->GetFramebuffer()->Bind();
		pipeline->GetShader()->Bind();

		auto& materialTable = model->GetMaterials();
		auto& envMaterial = s_Data.EnvironmentMaterial;

		s_Data.ModelBuffer.Transform = transform;
		s_Data.ModelBuffer.EntityID = entityID;
		s_Data.ModelDataBuffer->SetData(&s_Data.ModelBuffer, sizeof(RendererData::ModelData));

		envMaterial->Bind();

		for (auto& mesh : model->m_Meshes)
		{
			s_Data.Stats.VertexCount += mesh.m_Vertices.size();
			s_Data.Stats.IndexCount += mesh.m_Indices.size();
			s_Data.Stats.DrawCalls++;

			materialTable[mesh.m_MaterialIndex]->GetMaterial()->Bind();

			RenderCommand::DrawIndexed(mesh.m_VertexArray);
		}

		pipeline->GetFramebuffer()->Unbind();
	}

	void Renderer::RenderSelectedModel(const Ref<Pipeline>& pipeline, const Ref<Model>& model, const glm::mat4& transform, int entityID)
	{
		VS_PROFILE_FUNCTION();
	}

	Ref<SceneEnvironment> Renderer::CreateEmptyEnvironmentMap()
	{
		auto& defaultCube = Renderer::GetDefaultTextureCube();
		Ref<SceneEnvironment> emptyEnv = SceneEnvironment::Create(defaultCube, defaultCube);

		return emptyEnv;
	}

	Ref<SceneEnvironment> Renderer::CreateEnvironmentMap(const std::string& path)
	{
		CORE_LOG_INFO("Creating ENV MAP: {0}", path);

		const uint32_t cubeMapSize = 1024;
		const uint32_t irradianceMapSize = 32;

		// Load HDR Env
		TextureProperties props;
		props.WrapMode = TextureWrapMode::ClampToEdge;
		props.FlipVertically = false;
		Ref<Texture2D> hdrTexture = Texture2D::Create(path, props);

		// Convert Equirectangular to Cubemap
		TextureProperties cubeProps;
		cubeProps.Format = TextureFormat::RGBA32F;
		cubeProps.UseMipmaps = true;
		Ref<TextureCube> cubeMap = TextureCube::Create(cubeMapSize, cubeMapSize, cubeProps);
		{
			CORE_LOG_INFO("Converting Equirectangular to Cubemap");
			Ref<ComputePipeline> computePipeline = ComputePipeline::Create(Renderer::GetShaderLibrary()->Get("EquirectangularToCubeMap"));
			computePipeline->Begin();
			computePipeline->BindWriteOnlyImage(0, cubeMap);
			hdrTexture->Bind(1);
			computePipeline->Execute(cubeMapSize / 32, cubeMapSize / 32, 6);

			cubeMap->GenerateMips();
		}

		// Pre-filter mipmap levels
		Ref<TextureCube> filteredCubeMap = TextureCube::Create(cubeMapSize, cubeMapSize, cubeProps);
		{
			Ref<ComputePipeline> computePipeline = ComputePipeline::Create(Renderer::GetShaderLibrary()->Get("EnvironmentMipFilter"));
			uint32_t mipCount = filteredCubeMap->GetMipLevelCount();
			const float deltaRoughness = 1.0f / glm::max((float)mipCount - 1.0f, 1.0f);

			computePipeline->Begin();
			for (uint32_t i = 0, size = cubeMapSize; i < mipCount; i++, size /= 2)
			{
				uint32_t numGroups = glm::max(1u, size / 32);
				float roughness = i * deltaRoughness;
				roughness = glm::max(roughness, 0.05f);

				CORE_LOG_INFO("Mip: {0} / NumGroups for this pass: {1} / Roughness: {2}", i, numGroups, roughness);

				computePipeline->GetShader()->SetFloat("u_Uniforms.Roughness", roughness);
				computePipeline->BindWriteOnlyImage(0, filteredCubeMap, i);
				cubeMap->Bind(1);
				computePipeline->Execute(numGroups, numGroups, 6);
			}
		}

		// Irradiance Cube map
		Ref<TextureCube> irradianceCubeMap = TextureCube::Create(irradianceMapSize, irradianceMapSize, cubeProps);
		{
			Ref<ComputePipeline> computePipeline = ComputePipeline::Create(Renderer::GetShaderLibrary()->Get("EnvironmentIrradiance"));
			
			CORE_LOG_INFO("Irradiance Map Compute");

			computePipeline->Begin();
			computePipeline->GetShader()->SetFloat("u_Uniforms.Samples", 512);
			computePipeline->BindWriteOnlyImage(0, irradianceCubeMap);
			filteredCubeMap->Bind(1);
			computePipeline->Execute(irradianceMapSize / 32, irradianceMapSize / 32, 6);

			irradianceCubeMap->GenerateMips();
		}

		return SceneEnvironment::Create(filteredCubeMap, irradianceCubeMap);
	}

	Ref<TextureCube> Renderer::CreatePreethamSky(glm::vec3 TurbidityAzimuthInclination)
	{
		uint32_t cubeMapSize = 1024;

		TextureProperties cubeProps;
		cubeProps.Format = TextureFormat::RGBA32F;
		cubeProps.UseMipmaps = true;
		Ref<TextureCube> cubeMap = TextureCube::Create(cubeMapSize, cubeMapSize, cubeProps);

		Ref<ComputePipeline> computePipeline = ComputePipeline::Create(Renderer::GetShaderLibrary()->Get("PreethamSky"));

		computePipeline->Begin();
		computePipeline->GetShader()->SetFloat3("u_Uniforms.TurbidityAzimuthInclination", 
			{ TurbidityAzimuthInclination.x, TurbidityAzimuthInclination.y, TurbidityAzimuthInclination.z });

		computePipeline->BindWriteOnlyImage(0, cubeMap);
		computePipeline->Execute(cubeMapSize / 32, cubeMapSize / 32, 6);

		cubeMap->GenerateMips();

		return cubeMap;
	}

	void Renderer::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Renderer::Statistics));

		Renderer2D::ResetStats();
	}

	void Renderer::SetEnvironment(Ref<SceneEnvironment> envMap, uint32_t shadowMap)
	{
		auto& envMaterial = s_Data.EnvironmentMaterial;

		s_Data.EnviromentMap = envMap;
		s_Data.ShadowMap = shadowMap;

		envMaterial->SetCubeMap("u_EnvRadianceTex", 4, envMap->GetRadianceMap()->GetRendererID());
		envMaterial->SetCubeMap("u_EnvIrradianceTex", 5, envMap->GetIrradianceMap()->GetRendererID());
		envMaterial->SetTexture("u_BRDFLUTTexture", 6, s_Data.BRDFLutTexture->GetRendererID());
		envMaterial->SetTextureArray("u_ShadowMapTexture", 7, shadowMap);
	}

	Ref<Texture2D> Renderer::GetDefaultTexture()
	{
		return s_Data.DefaultTexture;
	}

	Ref<TextureCube> Renderer::GetDefaultTextureCube()
	{
		return s_Data.DefaultCube;
	}

	Ref<ShaderLibrary> Renderer::GetShaderLibrary()
	{
		return s_Data.ShaderLibrary;
	}

	float Renderer::GetDebugParam()
	{
		return s_Data.DebugParam;
	}

	void Renderer::SetDebugTexture(uint32_t textureID)
	{
		s_Data.DebugTexture = textureID;
	}

	void Renderer::OnImGuiRender()
	{
		ImGui::Begin("Renderer Debug");
		ImGui::DragFloat("Layer", &s_Data.DebugParam, 1.0f, 0.0f, 3.0f);
		glm::vec2 viewportPanelSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };
		if (s_Data.DebugTexture > 0)
		{
			ImGui::Image(reinterpret_cast<void*>(s_Data.DebugTexture), ImVec2{ 512, 512 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		}		
		ImGui::End();
	}

	Renderer::Statistics Renderer::GetStats()
	{
		return s_Data.Stats;
	}

}
