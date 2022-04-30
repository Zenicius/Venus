#include <Venus.h>
#include <Engine/EntryPoint.h>

#include "DevLayer.h"

class SandboxApp : public Venus::Application
{
	public:
		SandboxApp(Venus::ApplicationCommandLineArgs args)
			:Venus::Application("Venus Engine")
		{
			PushLayer(new DevLayer());
		}

		~SandboxApp()
		{
		}
};

Venus::Application* Venus::CreateApplication(Venus::ApplicationCommandLineArgs args)
{
	return new SandboxApp(args);
}
