#include "pch.h"

#include "Renderer/OpenGL/OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Venus {

	static const uint32_t s_MaxFramebufferSize = 8192;

	namespace Utils {

		static GLenum TextureTarget(bool multisampled, bool layered) // Multisampled has priority
		{
			if (multisampled)
				return GL_TEXTURE_2D_MULTISAMPLE;
			else if (layered)
				return GL_TEXTURE_2D_ARRAY;
			else
				return GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, bool layered, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled, layered), count, outID);
		}

		static void BindTexture(bool multisampled, bool layerred, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampled, layerred), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, int layers, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index, GLenum type)
		{
			bool multisampled = samples > 1;
			bool layered = layers > 1;

			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else if (layered)
			{
				VS_CORE_ASSERT(false, "Not implemented!");
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled, layered), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, int layers, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			bool layered = layers > 1;

			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else if (layered)
			{
				glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, format, width, height, layers);

				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			if(layered)
				glFramebufferTexture(GL_FRAMEBUFFER, attachmentType, id, 0);
			else
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled, layered), id, 0);
		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:		return true;
				case FramebufferTextureFormat::DEPTH32FSTENCIL8:	return true;
				case FramebufferTextureFormat::DEPTH24:				return true;
				case FramebufferTextureFormat::DEPTH32F:			return true;
				case FramebufferTextureFormat::STENCIL8:			return true;
			}

			return false;
		}

		static GLenum VenusFBTextureFormatToGL(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
				case FramebufferTextureFormat::RGBA16F:		return GL_RGBA16F;
				case FramebufferTextureFormat::RGBA32F:		return GL_RGBA32F;
				case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}

			VS_CORE_ASSERT(false);
			return 0;
		}

	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		for (auto spec : m_Specification.Attachments.Attachments)
		{
			if (!Utils::IsDepthFormat(spec.TextureFormat))
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);
			
			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Specification.Samples > 1;
		bool layered = m_Specification.Layers > 1;

		// Attachments
		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			Utils::CreateTextures(multisample, layered, m_ColorAttachments.data(), m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				Utils::BindTexture(multisample, layered, m_ColorAttachments[i]);
				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
					case FramebufferTextureFormat::RGBA8:
						Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, m_Specification.Layers, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i, GL_UNSIGNED_BYTE);
						break;
					case FramebufferTextureFormat::RGBA16F:
						Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, m_Specification.Layers, GL_RGBA16F, GL_RGBA, m_Specification.Width, m_Specification.Height, i, GL_FLOAT);
						break;
					case FramebufferTextureFormat::RGBA32F:
						Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, m_Specification.Layers, GL_RGBA32F, GL_RGBA, m_Specification.Width, m_Specification.Height, i, GL_FLOAT);
						break;
					case FramebufferTextureFormat::RED_INTEGER:
						Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, m_Specification.Layers, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i, GL_UNSIGNED_BYTE);
						break;
				}
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			Utils::CreateTextures(multisample, layered, &m_DepthAttachment, 1);
			Utils::BindTexture(multisample, layered, m_DepthAttachment);
			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:
					Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, m_Specification.Layers, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
					break;
				case FramebufferTextureFormat::DEPTH32FSTENCIL8:
					Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, m_Specification.Layers, GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
					break;
				case FramebufferTextureFormat::DEPTH24:
					Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, m_Specification.Layers, GL_DEPTH_COMPONENT24, GL_DEPTH_ATTACHMENT, m_Specification.Width, m_Specification.Height);
					break;
				case FramebufferTextureFormat::DEPTH32F:
					Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, m_Specification.Layers, GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, m_Specification.Width, m_Specification.Height);
					break;
				case FramebufferTextureFormat::STENCIL8:
					Utils::AttachDepthTexture(m_DepthAttachment, m_Specification.Samples, m_Specification.Layers, GL_STENCIL_INDEX8, GL_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
					break;
			}
		}

		if (m_ColorAttachments.size() > 1)
		{
			VS_CORE_ASSERT(m_ColorAttachments.size() <= 4);
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		VS_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			CORE_LOG_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	void OpenGLFramebuffer::Copy(const Ref<Framebuffer>& from, uint32_t width, uint32_t height)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, from->GetRendererID());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		VS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return pixelData;
	}

	void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		VS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());

		auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
			Utils::VenusFBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
	}

}
