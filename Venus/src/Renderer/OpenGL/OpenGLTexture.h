#pragma once

#include "Renderer/Texture.h"

#include <glad/glad.h>

namespace Venus {

	static GLenum OpenGLWrapMode(TextureWrapMode mode)
	{
		switch (mode)
		{
			case TextureWrapMode::Repeat:			return GL_REPEAT;
			case TextureWrapMode::Mirrored:			return GL_MIRRORED_REPEAT;
			case TextureWrapMode::ClampToEdge:		return GL_CLAMP_TO_EDGE;
			case TextureWrapMode::ClampToBorder:	return GL_CLAMP_TO_BORDER;
		}
	}

	static GLenum OpenGLFilterMode(TextureFilterMode mode)
	{
		switch (mode)
		{
			case TextureFilterMode::Point:		return GL_NEAREST;
			case TextureFilterMode::Bilinear:	return GL_LINEAR;
		}
	}

	static GLenum OpenGLTextureFormat(TextureFormat format)
	{
		switch (format)
		{
			case TextureFormat::SRGB:		return GL_SRGB8_ALPHA8;
			case TextureFormat::RGBA8:		return GL_RGBA8;
			case TextureFormat::RGBA16F:	return GL_RGBA16F;
			case TextureFormat::RGBA32F:	return GL_RGBA32F;
		}
	}

	class OpenGLTexture2D : public Texture2D
	{
		public:
			OpenGLTexture2D(uint32_t width, uint32_t height, TextureProperties props = TextureProperties());
			OpenGLTexture2D(const std::string& path, TextureProperties props = TextureProperties());
			virtual ~OpenGLTexture2D();

			virtual uint32_t GetWidth() const override { return m_Width;  }
			virtual uint32_t GetHeight() const override { return m_Height; }
			virtual uint32_t GetRendererID() const override { return m_RendererID; }

			virtual std::string GetPath() const override { return m_Path; }
		
			virtual void SetData(void* data, uint32_t size, uint32_t mipLevel = 0) override;
			virtual void GenerateMips() override;

			virtual void Bind(uint32_t slot = 0) const override;

			virtual bool IsLoaded() const override { return m_IsLoaded; }
			virtual TextureProperties GetProperties() const override { return m_Properties; }
			virtual TextureType GetType() const override { return TextureType::Texture2D; }
			virtual uint32_t GetMipLevelCount() const override;
			virtual std::pair<uint32_t, uint32_t> GetMipSize(uint32_t mip) const override;

			virtual bool operator==(const Texture& other) const override
			{
				return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
			}

		private:
			uint32_t m_RendererID;
			std::string m_Path;
			bool m_IsLoaded = false;

			TextureProperties m_Properties;
			uint32_t m_Width, m_Height;
			GLenum m_InternalFormat, m_DataFormat;
	};

	class OpenGLTextureCube : public TextureCube
	{
		public:
			OpenGLTextureCube(uint32_t width, uint32_t height, TextureProperties props = TextureProperties());
			OpenGLTextureCube(std::vector<std::string> paths, TextureProperties props = TextureProperties());
			virtual ~OpenGLTextureCube();

			virtual uint32_t GetWidth() const override { return m_Width; }
			virtual uint32_t GetHeight() const override { return m_Height; }
			virtual uint32_t GetRendererID() const override { return m_RendererID; }

			virtual std::string GetPath() const override { return m_Path; }

			virtual void SetData(void* data, uint32_t size, uint32_t mipLevel = 0) override;
			virtual void GenerateMips() override;

			virtual bool IsLoaded() const override { return m_IsLoaded; }
			virtual TextureProperties GetProperties() const override { return m_Properties; }
			virtual TextureType GetType() const override { return TextureType::TextureCube; }
			virtual uint32_t GetMipLevelCount() const override;
			virtual std::pair<uint32_t, uint32_t> GetMipSize(uint32_t mip) const override;

			virtual void Bind(uint32_t slot = 0) const override;

			virtual bool operator==(const Texture& other) const override
			{
				return m_RendererID == ((OpenGLTextureCube&)other).m_RendererID;
			}

		private:
			uint32_t m_RendererID;
			std::string m_Path;
			bool m_IsLoaded = false;
	
			TextureProperties m_Properties;
			uint32_t m_Width, m_Height;
			GLenum m_InternalFormat, m_DataFormat;
	};
}
