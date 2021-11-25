#include "pch.h"
#include "Application.h"

namespace Venus {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{

	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (m_Running)
		{
		}
	}

	void Application::Close()
	{
		m_Running = false;
	}
}
