#pragma once

#include "Platform.h"

namespace Venus
{
	class VENUS_API Application
	{
		public:
			Application();
			virtual ~Application();

			void Run();
	};

	// Client
	Application* CreateApplication();
}
