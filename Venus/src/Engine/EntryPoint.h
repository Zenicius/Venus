#pragma once
#include "Engine/Base.h"
#include "Engine/Application.h"

#ifdef VS_PLATFORM_WINDOWS

extern Venus::Application* Venus::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Venus::Log::Init();

	VS_PROFILE_BEGIN_SESSION("Startup", "VenusProfile-Startup.json");
	auto app = Venus::CreateApplication({ argc, argv });
	VS_PROFILE_END_SESSION();

	VS_PROFILE_BEGIN_SESSION("Runtime", "VenusProfile-Runtime.json");
	app->Run();
	VS_PROFILE_END_SESSION();

	VS_PROFILE_BEGIN_SESSION("Shutdown", "VenusProfile-Shutdown.json");
	CORE_LOG_WARN("Shutting Down...");
	delete app;
	VS_PROFILE_END_SESSION();
}

#endif
