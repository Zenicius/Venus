#pragma once

#ifdef VS_PLATFORM_WINDOWS
	#ifdef VS_BUILD_DLL
		#define VENUS_API __declspec(dllexport)
	#else
		#define VENUS_API __declspec(dllimport)
	#endif
#else

#endif