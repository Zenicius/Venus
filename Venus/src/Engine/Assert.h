#pragma once

#include "Base.h"
#include "Log.h"
#include <filesystem>

#ifdef VS_ENABLE_ASSERTS

// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define VS_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { type##LOG_##ERROR(msg, __VA_ARGS__); VS_DEBUGBREAK(); } }
#define VS_INTERNAL_ASSERT_WITH_MSG(type, check, ...) VS_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define VS_INTERNAL_ASSERT_NO_MSG(type, check) VS_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", VS_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define VS_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define VS_INTERNAL_ASSERT_GET_MACRO(...) VS_EXPAND_MACRO( VS_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, VS_INTERNAL_ASSERT_WITH_MSG, VS_INTERNAL_ASSERT_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define VS_ASSERT(...) VS_EXPAND_MACRO( VS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define VS_CORE_ASSERT(...) VS_EXPAND_MACRO( VS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(CORE_, __VA_ARGS__) )
#else
#define VS_ASSERT(...)
#define VS_CORE_ASSERT(...)
#endif
