#include "pch.h"

#include "Renderer/Framebuffer.h"
#include "Renderer/Renderer.h"
#include "Renderer/OpenGL/OpenGLFramebuffer.h"

namespace Venus {
	
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    VS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFramebuffer>(spec);
		}

		VS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}

