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

		WindowProps(const std::string& title = "Venus Engine",
			uint32_t width = 1920,
			uint32_t height = 1080)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
		public:
			using EventCallbackFn = std::function<void(Event&)>;

			virtual ~Window() = default;

			virtual void OnUpdate() = 0;

			virtual uint32_t GetWidth() const = 0;
			virtual uint32_t GetHeight() const = 0;

			// Window attributes
			virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
			virtual void SetVSync(bool enabled) = 0;
			virtual bool IsVSync() const = 0;
			virtual void SetWindowTitle(const std::string& title) = 0;

			virtual void* GetNativeWindow() const = 0;

			static Scope<Window> Create(const WindowProps& props = WindowProps());
	};

}
