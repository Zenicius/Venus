#include <Venus.h>
#include <Engine/EntryPoint.h>

#include "TestLayer.h"

class SandboxApp : public Venus::Application
{
	public:
		SandboxApp(Venus::ApplicationCommandLineArgs args)
			:Venus::Application("Venus - Sandbox")
		{
			PushLayer(new TestLayer());
		}

		~SandboxApp()
		{
		}
};

Venus::Application* Venus::CreateApplication(Venus::ApplicationCommandLineArgs args)
{
	return new SandboxApp(args);
}
