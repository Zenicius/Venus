#pragma once

#include <sstream>

#include "Engine/Base.h"
#include "Events/Event.h"

namespace Venus {

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		bool Vsync;
		bool Fullscreen;
		bool Decorated;

		WindowProps(const std::string& title = "Venus Engine",
			uint32_t width = 1600,
			uint32_t height = 900,
			bool vSync = true,
			bool fullscreen = true,
			bool decorated = true)
			: Title(title), Width(width), Height(height), Vsync(vSync), Fullscreen(fullscreen), Decorated(decorated)
		{
		}
	};

	class Window
	{
		public:
			using EventCallbackFn = std::function<void(Event&)>;

			virtual ~Window() = default;

			virtual void OnUpdate() = 0;

			virtual uint32_t GetWidth() const = 0;
			virtual uint32_t GetHeight() const = 0;

			virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

			virtual void SetVSync(bool enabled) = 0;
			virtual bool IsVSync() const = 0;

			virtual void SetWindowTitle(const std::string& title) = 0;

			virtual void Maximize() = 0;
			virtual bool IsMaximized() = 0;
			virtual void Minimize() = 0;
			virtual void Restore() = 0;

			virtual void* GetNativeWindow() const = 0;

			static Scope<Window> Create(const WindowProps& props = WindowProps());
	};

}
