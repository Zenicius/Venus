#pragma once

#include "Engine/Base.h"
#include "Engine/Window.h"
#include "Engine/LayerStack.h"
#include "Engine/Timestep.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Venus {

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			VS_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Venus App";
		uint32_t Width = 1920, Height = 1080;
		bool Fullscreen = true;
		bool Vsync = true;
		bool WindowDecorated = true;
	};

	class Application
	{
		public:
			Application(ApplicationSpecification spec = ApplicationSpecification(), ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
			virtual ~Application();

			void Close();
			void OnEvent(Event& e);

			void PushLayer(Layer* layer);
			void PushOverlay(Layer* layer);

			static Application& Get() { return *s_Instance; }
			Window& GetWindow() { return *m_Window; }
			ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
			float GetFrametime() { return m_Timestep.GetMilliseconds(); }
			ApplicationCommandLineArgs GetCommandLineArgs() const { return m_CommandLineArgs; }

		private:
			void Run();
			bool OnWindowClose(WindowCloseEvent& e);
			bool OnWindowResize(WindowResizeEvent& e);
		private:
			ApplicationCommandLineArgs m_CommandLineArgs;
			Scope<Window> m_Window;
			ImGuiLayer* m_ImGuiLayer;
			bool m_Running = true;
			bool m_Minimized = false;
			LayerStack m_LayerStack;
			Timestep m_Timestep;
			float m_LastFrameTime = 0.0f;


		//--------------------------------------------------------------------------------------------
		private:
			static Application* s_Instance;
			friend int ::main(int argc, char** argv);
	};

	// CLIENT APP
	Application* CreateApplication(ApplicationCommandLineArgs args);
}
