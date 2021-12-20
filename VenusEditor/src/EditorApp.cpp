#include <Venus.h>
#include <Engine/EntryPoint.h>

#include "EditorLayer.h"

namespace Venus {

	class EditorApp : public Application
	{
		public:
			EditorApp(ApplicationCommandLineArgs args)
				:Application("Venus Editor")
			{
				PushLayer(new EditorLayer());
			}

			~EditorApp()
			{
			}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new EditorApp(args);
	}

}