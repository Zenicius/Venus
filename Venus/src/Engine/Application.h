#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Engine/Window.h"
#include "Engine/LayerStack.h"
#include "Engine/Timestep.h"

namespace Venus
{
	class Application
	{
		public:
			Application();
			virtual ~Application();

			void Init();
			void PushLayer(Layer* layer);
			void PushOverlay(Layer* overlay);

			void Run();
			void Close();


			static Application& Get() { return *s_Instance; }

		private:
			static Application* s_Instance;

			Window* m_Window;
			LayerStack m_LayerStack;
			
			bool m_Running = true;
			float m_LastFrameTime = 0.0f;
	};

	// Client
	Application* CreateApplication();
}
