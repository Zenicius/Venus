#pragma once

#include "Application.h"

extern Venus::Application* Venus::CreateApplication();

int main(int argc, char** argv)
{
	// Move to Engine init function
	Venus::Log::Init();

	auto app = Venus::CreateApplication();
	app->Run();
	CORE_LOG_WARN("Shutting down...");
	delete app;
}

