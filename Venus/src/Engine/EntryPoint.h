#pragma once

#ifdef VS_PLATFORM_WINDOWS

extern Venus::Application* Venus::CreateApplication();

int main(int argc, char** argv)
{
	// Move to Engine init function
	Venus::Log::Init();

	auto app = Venus::CreateApplication();
	app->Run();
	delete app;
}

#endif