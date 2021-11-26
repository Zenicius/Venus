workspace "Venus"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Venus/vendor/GLFW/include"
IncludeDir["Glad"] = "Venus/vendor/Glad/include"
IncludeDir["glm"] = "Venus/vendor/glm"
IncludeDir["stb_image"] = "Venus/vendor/stb_image"

group "Dependencies"
	include "Venus/vendor/GLFW"
	include "Venus/vendor/Glad"

group ""

project "Venus"
	location "Venus"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "Venus/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "VS_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "VS_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "VS_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Venus/vendor/spdlog/include",
		"Venus/src",
		"Venus/vendor",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"
	}

	links
	{
		"Venus"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "VS_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "VS_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "VS_DIST"
		runtime "Release"
		optimize "on"