#include "pch.h"
#include "Renderer/RendererAPI.h"

#include <Renderer/OpenGL/OpenGLRendererAPI.h>

namespace Venus {

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	std::string RendererAPI::GetAPIName()
	{
		switch (s_API)
		{
			case RendererAPI::API::None:    "";
			case RendererAPI::API::OpenGL:  return "OpenGL";
		}
	}

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
			case RendererAPI::API::None:    VS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRendererAPI>();
		}

		VS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}