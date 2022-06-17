#include "pch.h"
#include "ComputePipeline.h"

#include "Renderer/Renderer.h"
#include "Renderer/OpenGL/OpenGLComputePipeline.h"

namespace Venus {

	Ref<ComputePipeline> ComputePipeline::Create(Ref<Shader> computeShader)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    VS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLComputePipeline>(computeShader);
		}

		VS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}