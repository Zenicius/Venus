#include "pch.h"
#include "OpenGLComputePipeline.h"
#include "OpenGLTexture.h"

#include "Engine/Timer.h"

#include <glad/glad.h>

namespace Venus {

	OpenGLComputePipeline::OpenGLComputePipeline(Ref<Shader> computeShader)
		: m_Shader(computeShader)
	{
	}

	OpenGLComputePipeline::~OpenGLComputePipeline()
	{
	}

	void OpenGLComputePipeline::Begin()
	{
		m_Shader->Bind();
	}

	void OpenGLComputePipeline::Execute(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ, bool LogExecuteTime)
	{
		Timer timer;
	
		glDispatchCompute(groupCountX, groupCountY, groupCountZ);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		if(LogExecuteTime)
			CORE_LOG_WARN("Compute shader took: {0}ms", timer.ElapsedMillis());
	}

	void OpenGLComputePipeline::BindImage(uint32_t binding, Ref<Texture> texture, uint32_t level, uint32_t layer)
	{
		bool layered = false;
		if (texture->GetType() == TextureType::TextureCube)
			layered = true;

		GLenum format = OpenGLTextureFormat(texture->GetProperties().Format);

		glBindImageTexture(binding, texture->GetRendererID(), level, layered, layer, GL_READ_WRITE, format);
	}

	void OpenGLComputePipeline::BindReadOnlyImage(uint32_t binding, Ref<Texture> texture, uint32_t level, uint32_t layer)
	{
		bool layered = false;
		if (texture->GetType() == TextureType::TextureCube)
			layered = true;

		GLenum format = OpenGLTextureFormat(texture->GetProperties().Format);

		glBindImageTexture(binding, texture->GetRendererID(), level, layered, layer, GL_READ_ONLY, format);
	}

	void OpenGLComputePipeline::BindWriteOnlyImage(uint32_t binding, Ref<Texture> texture, uint32_t level, uint32_t layer)
	{
		bool layered = false;
		if (texture->GetType() == TextureType::TextureCube)
			layered = true;

		GLenum format = OpenGLTextureFormat(texture->GetProperties().Format);

		glBindImageTexture(binding, texture->GetRendererID(), level, layered, layer, GL_WRITE_ONLY, format);
	}

}