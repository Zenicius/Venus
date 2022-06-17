#include "pch.h"
#include "Renderer/Texture.h"

#include "Renderer/Renderer.h"
#include "Renderer/OpenGL/OpenGLTexture.h"

namespace Venus {

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, TextureProperties props)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    VS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(width, height, props);
		}

		VS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path, TextureProperties props)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    VS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path, props);
		}

		VS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(uint32_t width, uint32_t height, TextureProperties props)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    VS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTextureCube>(width, height, props);
		}

		VS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<TextureCube> TextureCube::Create(std::vector<std::string> paths, TextureProperties props)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    VS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTextureCube>(paths, props);
		}

		VS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}