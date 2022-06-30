#include "pch.h"
#include "Engine/Application.h"

#include "Engine/Log.h"
#include "Engine/Input.h"
#include "Renderer/Renderer.h"
#include "Scripting/ScriptingEngine.h"

#include <GLFW/glfw3.h>

namespace Venus {

	Application* Application::s_Instance = nullptr;

	Application::Application(ApplicationSpecification spec, ApplicationCommandLineArgs args)
		: m_CommandLineArgs(args)
	{
		VS_PROFILE_FUNCTION();
		VS_CORE_ASSERT(!s_Instance, "Application already exists!");

		s_Instance = this;

		// Window
		WindowProps props;
		props.Title = spec.Name;
		props.Width = spec.Width;
		props.Height = spec.Height;
		props.Fullscreen = spec.Fullscreen;
		props.Vsync = spec.Vsync;
		props.Decorated = spec.WindowDecorated;
		m_Window = Window::Create(props);
		m_Window->SetEventCallback(VS_BIND_EVENT_FN(Application::OnEvent));
	
		// Init Engine Components
		Renderer::Init();
		ScriptingEngine::Init();

		// ImGui
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		VS_PROFILE_FUNCTION();

		m_Window->SetEventCallback([](Event& e) {});
		
		Renderer::Shutdown();
		ScriptingEngine::Shutdown();
		Log::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		VS_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		VS_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::OnEvent(Event& e)
	{
		VS_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(VS_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(VS_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		VS_PROFILE_FUNCTION();

		while (m_Running)
		{
			VS_PROFILE_SCOPE("RunLoop");

			float time = (float)glfwGetTime();
			m_Timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				{
					VS_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(m_Timestep);
				}

				m_ImGuiLayer->Begin();
				{
					VS_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		VS_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}
