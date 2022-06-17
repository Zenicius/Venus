#pragma once

#include <glm/glm.hpp>

#include "KeyCodes.h"
#include "MouseCodes.h"

namespace Venus {

	enum class CursorMode
	{
		Normal = 0,
		Hidden = 1,
		Locked = 2
	};

	class Input
	{
		public:
			static bool IsKeyPressed(KeyCode key);

			static bool IsMouseButtonPressed(MouseCode button);
			static glm::vec2 GetMousePosition();
			static float GetMouseX();
			static float GetMouseY();
			static void SetCursorMode(CursorMode mode);
	};
}
