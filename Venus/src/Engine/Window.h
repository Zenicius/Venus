#pragma once

#include "pch.h"
#include <GLFW/glfw3.h>

namespace Venus {

	class Window
	{
		public:
			Window(unsigned int widht, unsigned int height, const std::string& title = "Venus Engine", bool VSync = true);
			~Window();

			void OnUpdate();

			GLFWwindow* GetWindow() const { return m_Window; }
			const std::string GetTitle() const { return m_Data.title; }
			const unsigned int GetWidth() const { return m_Data.Width; }
			const unsigned int GetHeight() const { return m_Data.Height; }
			void SetVSync(bool enabled);
			const bool IsVSync() const { return m_Data.VSync; }

		private:
			GLFWwindow* m_Window;

			void Init();

			struct WindowData
			{
				unsigned int Width, Height;
				std::string title;
				bool VSync;
			};

			WindowData m_Data;
		};
}