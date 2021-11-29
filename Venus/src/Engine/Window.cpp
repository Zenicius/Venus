#include "pch.h"
#include "Window.h"

namespace Venus {

	Window::Window(unsigned int widht, unsigned int height, const std::string& title, bool VSync)
	{
		m_Data.Width = widht;
		m_Data.Height = height;
		m_Data.title = title;
		m_Data.VSync = VSync;

		Init();
	}

	Window::~Window()
	{
		/*
		*
		* Delete window before ending the program
		* Terminate GLFW before ending the program
		*
		*/
		CORE_LOG_WARN("Destroying window...");
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Window::Init()
	{
		/*
		*
		* Inicialize GLFW and set the OpenGL version we are using (3.3)
		* Set OpenGL to use CORE PROFILE, so we have modern functions.
		*
		*/
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		int monitorCount;
		GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitors[0]);
		int monitorX, monitorY;
		glfwGetMonitorPos(monitors[0], &monitorX, &monitorY);

		/*
		*
		* Creates our window with parameters given (size, name) and centered.
		* Checks if window failed to create
		*
		*/
		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.title.c_str(), NULL, NULL);
		if (m_Window == NULL)
		{
			CORE_LOG_CRITICAL("Failed to inicialize GLFW Window!");
			glfwTerminate();
			return;
		}
		glfwSetWindowPos(m_Window, monitorX + (videoMode->width - m_Data.Width) / 2,
			monitorY + (videoMode->height - m_Data.Height) / 2);

		/*
		*
		* Pass the window created to the current context
		* Sets VSync on or off then show the window
		*
		*/
		glfwMakeContextCurrent(m_Window);

		if (m_Data.VSync)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		glfwShowWindow(m_Window);

		CORE_LOG_INFO("Inicialized {0}x{1} Window with VSync: {2}", GetWidth(), GetHeight(), IsVSync());
	}

	void Window::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void Window::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}


}