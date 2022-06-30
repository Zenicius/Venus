#include "pch.h"
#include "OpenGLTexture.h"

#include <stb_image.h>
#include "glm/glm.hpp"

namespace Venus {

	/////////////////////////////////////////////////////////////////////////////
	// Texture 2D ///////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, TextureProperties props)
		: m_Width(width), m_Height(height), m_Properties(props)
	{
		m_InternalFormat = OpenGLTextureFormat(props.Format);
		m_DataFormat = GL_RGBA;

		uint32_t mipmapCount = props.GenerateMipmaps ? GetMipLevelCount() : 1;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, mipmapCount, m_InternalFormat, m_Width, m_Height);

		GLenum minFilter = OpenGLFilterMode(props.Filter);
		if (props.UseMipmaps && minFilter == GL_LINEAR)
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
		else if (props.UseMipmaps && minFilter == GL_NEAREST)
			minFilter = GL_NEAREST_MIPMAP_NEAREST;
		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, OpenGLFilterMode(props.Filter));

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, OpenGLWrapMode(props.WrapMode));
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, OpenGLWrapMode(props.WrapMode));

		if(props.GenerateMipmaps)
			GenerateMips();
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path, TextureProperties props)
		: m_Path(path), m_Properties(props)
	{
		int width, height, channels;

		//-- HDR Image------------------------------------------------------------------------------
		if (stbi_is_hdr(path.c_str()))
		{
			stbi_set_flip_vertically_on_load(m_Properties.FlipVertically);
			float* data = nullptr;
			data = stbi_loadf(path.c_str(), &width, &height, &channels, 4);

			CORE_LOG_INFO("Texture is HDR!");

			if (data)
			{
				m_IsLoaded = true;

				m_Width = width;
				m_Height = height;

				props.Format = TextureFormat::RGBA32F;

				uint32_t mipmapCount = props.GenerateMipmaps ? GetMipLevelCount() : 1;

				m_InternalFormat = OpenGLTextureFormat(props.Format);
				m_DataFormat = GL_RGBA;

				glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
				glTextureStorage2D(m_RendererID, mipmapCount, m_InternalFormat, m_Width, m_Height);

				GLenum minFilter = OpenGLFilterMode(props.Filter);
				if (props.UseMipmaps && minFilter == GL_LINEAR)
					minFilter = GL_LINEAR_MIPMAP_LINEAR;
				else if (props.UseMipmaps && minFilter == GL_NEAREST)
					minFilter = GL_NEAREST_MIPMAP_NEAREST;
				glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
				glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, OpenGLFilterMode(props.Filter));

				glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, OpenGLWrapMode(props.WrapMode));
				glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, OpenGLWrapMode(props.WrapMode));

				glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_FLOAT, data);

				if (props.GenerateMipmaps)
					GenerateMips();

				stbi_image_free(data);
			}
			else
				CORE_LOG_ERROR("Could not find HDR texture: {0}", path);
		}
		//-- Normal Image---------------------------------------------------------------------------
		else
		{
			stbi_set_flip_vertically_on_load(m_Properties.FlipVertically);
			stbi_uc* data = nullptr;
			data = stbi_load(path.c_str(), &width, &height, &channels, 4);

			if (data)
			{
				m_IsLoaded = true;

				m_Width = width;
				m_Height = height;

				m_InternalFormat = OpenGLTextureFormat(props.Format);
				m_DataFormat = GL_RGBA;

				uint32_t mipmapCount = props.GenerateMipmaps ? GetMipLevelCount() : 1;

				glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
				glTextureStorage2D(m_RendererID, mipmapCount, m_InternalFormat, m_Width, m_Height);

				GLenum minFilter = OpenGLFilterMode(props.Filter);
				if (props.UseMipmaps && minFilter == GL_LINEAR)
					minFilter = GL_LINEAR_MIPMAP_LINEAR;
				else if (props.UseMipmaps && minFilter == GL_NEAREST)
					minFilter = GL_NEAREST_MIPMAP_NEAREST;
				glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, minFilter);
				glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, OpenGLFilterMode(props.Filter));

				glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, OpenGLWrapMode(props.WrapMode));
				glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, OpenGLWrapMode(props.WrapMode));

				glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);

				if (props.GenerateMipmaps)
					GenerateMips();

				stbi_image_free(data);
			}
			else
				CORE_LOG_ERROR("Could not find texture: {0}", path);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size, uint32_t mipLevel)
	{
		uint32_t desiredSize;
		uint32_t width = m_Width;
		uint32_t height = m_Height;
		if (mipLevel > 0)
		{
			auto [mipWidth, mipHeight] = GetMipSize(mipLevel);
			desiredSize = mipWidth * mipHeight * 4;

			width = mipWidth;
			height = mipHeight;
		}
		else
			desiredSize = m_Width * m_Height * 4;

		VS_CORE_ASSERT(size == desiredSize, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, mipLevel, 0, 0, width, height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::GenerateMips()
	{
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	uint32_t OpenGLTexture2D::GetMipLevelCount() const
	{
		return (uint32_t)std::floor(std::log2(glm::min(m_Width, m_Height))) + 1;
	}

	std::pair<uint32_t, uint32_t> OpenGLTexture2D::GetMipSize(uint32_t mip) const
	{
		uint32_t width = m_Width;
		uint32_t height = m_Height;

		while (mip != 0)
		{
			width /= 2;
			height /= 2;
			mip--;
		}

		return {width, height};
	}

	/////////////////////////////////////////////////////////////////////////////
	// Texture Cube /////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	OpenGLTextureCube::OpenGLTextureCube(uint32_t width, uint32_t height, TextureProperties props)
		: m_Width(width), m_Height(height), m_Properties(props)
	{
		m_InternalFormat = OpenGLTextureFormat(props.Format);
		m_DataFormat = GL_RGBA;

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		for (uint32_t i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_FLOAT, nullptr); // Const FLOAT?
		}

		GLenum minFilter = OpenGLFilterMode(props.Filter);
		if (props.UseMipmaps && minFilter == GL_LINEAR)
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
		else if (props.UseMipmaps && minFilter == GL_NEAREST)
			minFilter = GL_NEAREST_MIPMAP_NEAREST;
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, OpenGLFilterMode(props.Filter));

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, OpenGLWrapMode(props.WrapMode));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, OpenGLWrapMode(props.WrapMode));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, OpenGLWrapMode(props.WrapMode));

		if (props.GenerateMipmaps)
			GenerateMips();
	}

	OpenGLTextureCube::OpenGLTextureCube(std::vector<std::string> paths, TextureProperties props)
	{
		m_InternalFormat = OpenGLTextureFormat(props.Format);
		m_DataFormat = GL_RGBA;

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

		int width, height, channels;

		for (uint32_t i = 0; i < paths.size(); i++)
		{
			stbi_uc* data = nullptr;
			data = stbi_load(paths[i].c_str(), &width, &height, &channels, 4);

			if (data)
			{
				m_Width = width;
				m_Height = height;

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
				CORE_LOG_ERROR("Could not find texture: {0}", paths[i]); 
		}

		GLenum minFilter = OpenGLFilterMode(props.Filter);
		if (props.UseMipmaps && minFilter == GL_LINEAR)
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
		else if (props.UseMipmaps && minFilter == GL_NEAREST)
			minFilter = GL_NEAREST_MIPMAP_NEAREST;
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, OpenGLFilterMode(props.Filter));

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, OpenGLWrapMode(props.WrapMode));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, OpenGLWrapMode(props.WrapMode));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, OpenGLWrapMode(props.WrapMode));

		if (props.GenerateMipmaps)
			GenerateMips();
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTextureCube::SetData(void* data, uint32_t size, uint32_t mipLevel)
	{
		// TODO: Check if data is entire texture / SetData in mip levels...
		// Same data for all faces for now...
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
		for (uint32_t i = 0; i < 6; ++i)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_FLOAT, data); // Const FLOAT?
		}
	}

	void OpenGLTextureCube::GenerateMips()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	void OpenGLTextureCube::Bind(uint32_t slot) const
	{
		glBindTextureUnit(slot, m_RendererID);
	}

	uint32_t OpenGLTextureCube::GetMipLevelCount() const
	{
		return (uint32_t)std::floor(std::log2(glm::min(m_Width, m_Height))) + 1;
	}

	std::pair<uint32_t, uint32_t> OpenGLTextureCube::GetMipSize(uint32_t mip) const
	{
		uint32_t width = m_Width;
		uint32_t height = m_Height;

		while (mip != 0)
		{
			width /= 2;
			height /= 2;
			mip--;
		}

		return { width, height };
	}
}
