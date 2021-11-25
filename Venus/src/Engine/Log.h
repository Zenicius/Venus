#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


namespace Venus {

	class Log
	{
		public:
			static void Init();

			inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
			inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		private:
			static std::shared_ptr<spdlog::logger> s_CoreLogger;
			static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// CORE LOG MACROS
#define CORE_LOG_TRACE(...)			::Venus::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_LOG_INFO(...)			::Venus::Log::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_LOG_WARN(...)			::Venus::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_LOG_ERROR(...)			::Venus::Log::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_LOG_CRITICAL(...)		::Venus::Log::GetCoreLogger()->critical(__VA_ARGS__)

// CLIENT LOG MACROS
#define LOG_TRACE(...)				::Venus::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)				::Venus::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)				::Venus::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)				::Venus::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)			::Venus::Log::GetClientLogger()->critical(__VA_ARGS__)

