#include "pch.h"
#include "Window.h"

#ifdef VS_PLATFORM_WINDOWS
#include "Engine/Platform/WindowsWindow.h"
#endif

namespace Venus {

	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef VS_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#else
		VS_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

}