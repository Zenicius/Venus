#pragma once

namespace Venus {

	class GraphicsContext
	{
		public:
			struct ContextInfo
			{
				std::string Vendor = std::string();
				std::string Renderer = std::string();
				std::string Version = std::string();
			};

			virtual ~GraphicsContext() = default;

			virtual void Init() = 0;
			virtual void SwapBuffers() = 0;

			static Scope<GraphicsContext> Create(void* window);
	};

}