#include "pch.h"
#include "Renderer/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Renderer/RendererAPI.h"

namespace Venus {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		VS_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		VS_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		VS_CORE_ASSERT(status, "Failed to initialize Glad!");

		std::string vendor = (const char*)glGetString(GL_VENDOR);
		std::string renderer = (const char*)glGetString(GL_RENDERER);
		std::string version = (const char*)glGetString(GL_VERSION);

		CORE_LOG_INFO("OpenGL Info:");
		CORE_LOG_INFO("  Vendor: {0}", vendor);
		CORE_LOG_INFO("  Renderer: {0}", renderer);
		CORE_LOG_INFO("  Version: {0}", version);


		VS_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Venus requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::SwapBuffers()
	{
		VS_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}

}
