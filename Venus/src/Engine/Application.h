#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Engine/Window.h"
#include "Engine/LayerStack.h"
#include "Engine/Timestep.h"

#include "ImGui/ImGuiLayer.h"

#include "Renderer/Renderer2D.h"

int main(int argc, char** argv);

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

			Window& GetWindow() { return *m_Window; }
			static Application& Get() { return *s_Instance; }

		private:
			static Application* s_Instance;

			Window* m_Window;
			LayerStack m_LayerStack;

			ImGuiLayer* m_ImGuiLayer;
			
			bool m_Running = true;
			float m_LastFrameTime = 0.0f;

			friend int ::main(int argc, char** argv);
	};

	// Client
	Application* CreateApplication();
}
