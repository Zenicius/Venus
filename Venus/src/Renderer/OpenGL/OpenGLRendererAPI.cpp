#include "pch.h"
#include "Renderer/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Venus {
	
	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:         CORE_LOG_CRITICAL(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:       CORE_LOG_ERROR(message); return;
			case GL_DEBUG_SEVERITY_LOW:          CORE_LOG_WARN(message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: CORE_LOG_TRACE(message); return;
		}
		
		VS_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init()
	{
		VS_PROFILE_FUNCTION();

	#ifdef VS_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);
		
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_LINE_SMOOTH);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DisableStencilTest()
	{
		glDisable(GL_STENCIL_TEST);
	}

	void OpenGLRendererAPI::EnableStencilTest()
	{
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
	}

	void OpenGLRendererAPI::DisableStencilWrite()
	{
		glStencilMask(0x00);
	}

	void OpenGLRendererAPI::EnableStencilWrite()
	{
		glStencilMask(0xFF);
	}

	void OpenGLRendererAPI::DisableDepthTest()
	{
		glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::EnableDepthTest()
	{
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetStencilTest(int function, int value, int mask)
	{
		glStencilFunc((GLenum)function, value, mask);
	}

	void OpenGLRendererAPI::BindTexture(int textureID)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	void OpenGLRendererAPI::BindFramebuffer(int framebufferID)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRendererAPI::SetLineWidth(float width)
	{
		// GLOBAL
		glLineWidth(width);
	}

}
