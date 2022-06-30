#include "pch.h"

#include "Editor/ConsolePanel.h"

namespace Venus {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		// CORE
		s_CoreLogger = spdlog::stdout_color_mt("VENUS");
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->set_pattern("%^[%T] %n: %v%$");

		// CLIENT
		std::vector<spdlog::sink_ptr> clientSinks =
		{
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
			std::make_shared<ConsoleSink>(1)
		};

		clientSinks[0]->set_pattern("%^[%T] %n: %v%$");
		clientSinks[1]->set_pattern("%^[%T] %v%$");

		s_ClientLogger = std::make_shared<spdlog::logger>("CLIENT", clientSinks.begin(), clientSinks.end());
		s_ClientLogger->set_level(spdlog::level::trace);

		CORE_LOG_INFO("Initialized Loggers!");
	}

	void Log::Shutdown()
	{
		s_CoreLogger.reset();
		s_ClientLogger.reset();
		spdlog::drop_all();
	}
}