#include "pch.h"
#include "Application.h"

namespace Venus {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;
		m_Window = new Window(1280, 720);

		Init();
	}

	Application::~Application()
	{
		delete m_Window;
	}

	void Application::Init()
	{
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			CORE_LOG_CRITICAL("Failed to initialize GLAD");
			return;
		}

		const GLubyte* version = glGetString(GL_VERSION);
		const GLubyte* vendor = glGetString(GL_VENDOR);
		const GLubyte* renderer = glGetString(GL_RENDERER);

		CORE_LOG_INFO("OpenGL Info:\n		Vendor: {0}\n		Version: {1}\n		Graphics: {2}",
			vendor, version, renderer);
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(m_Window->GetWindow()))
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(timestep);

			m_Window->OnUpdate();
		}
	}

	void Application::Close()
	{
		m_Running = false;
	}

}
