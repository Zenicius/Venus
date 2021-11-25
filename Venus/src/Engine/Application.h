#pragma once

namespace Venus
{
	class Application
	{
		public:
			Application();
			virtual ~Application();

			void Run();
			void Close();

			static Application& Get() { return *s_Instance; }

		private:
			static Application* s_Instance;
			
			bool m_Running = true;
	};

	// Client
	Application* CreateApplication();
}
