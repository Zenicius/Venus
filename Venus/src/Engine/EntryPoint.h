#pragma once

#ifdef VS_PLATFORM_WINDOWS

extern Venus::Application* Venus::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Venus::CreateApplication();
	app->Run();
	delete app;
}

#endif