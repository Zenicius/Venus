#include <Venus.h>
#include <Engine/EntryPoint.h>

#include "EditorLayer.h"

namespace Venus {

	class EditorApp : public Application
	{
		public:
			EditorApp(ApplicationSpecification spec,  ApplicationCommandLineArgs args)
				:Application(spec, args)
			{
				PushLayer(new EditorLayer());
			}

			~EditorApp()
			{
			}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Venus Editor";
		spec.Width = 1600;
		spec.Height = 900;
		spec.Fullscreen = true;
		spec.Vsync = false;
		spec.WindowDecorated = false;

		return new EditorApp(spec, args);
	}

}