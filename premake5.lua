workspace "Venus"
	architecture "x64"
	startproject "VenusEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

VULKAN_SDK = os.getenv("VULKAN_SDK")

-- INCLUDES
-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["Assimp"] = "Venus/vendor/assimp/include"
IncludeDir["AssimpBuild"] = "Venus/vendor/assimp/build/include"
IncludeDir["GLFW"] = "Venus/vendor/GLFW/include"
IncludeDir["Glad"] = "Venus/vendor/Glad/include"
IncludeDir["ImGui"] = "Venus/vendor/imgui"
IncludeDir["glm"] = "Venus/vendor/glm"
IncludeDir["stb_image"] = "Venus/vendor/stb_image"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["entt"] = "Venus/vendor/entt/include"
IncludeDir["yaml_cpp"] = "Venus/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "Venus/vendor/ImGuizmo"
IncludeDir["Box2D"] = "Venus/vendor/Box2D/include"
IncludeDir["mono"] = "Venus/vendor/mono/include"

--LIBS
LibraryDir = {}

--DIRS
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_DebugDLL"] = "%{VULKAN_SDK}/Bin"

Library = {}
-- assimp
Library["Assimp_Debug"] = "%{wks.location}/Venus/vendor/assimp/build/lib/Debug/assimp-vc142-mtd.lib"
Library["Assimp_Release"] = "%{wks.location}/Venus/vendor/assimp/build/lib/Release/assimp-vc142-mt.lib"

-- Mono
Library["mono"] = "%{wks.location}/Venus/vendor/mono/lib/Release/mono-2.0-sgen.lib"

-- Vulkan
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

-- Debug
Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

-- Release
Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

--BINS
Binaries = {}
Binaries["Assimp_Debug"] = "%{wks.location}/Venus/vendor/assimp/build/bin/Debug/assimp-vc142-mtd.dll"
Binaries["Assimp_Release"] = "%{wks.location}/Venus/vendor/assimp/build/bin/Release/assimp-vc142-mt.dll"

group "Dependencies"
	include "Venus/vendor/GLFW"
	include "Venus/vendor/Glad"
	include "Venus/vendor/imgui"
	include "Venus/vendor/yaml-cpp"
	include "Venus/vendor/Box2D"
group ""

-- VENUS ENGINE PROJECT 
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
		"%{prj.name}/src/**.c", 
		"%{prj.name}/src/**.hpp", 
		"%{prj.name}/src/**.cpp",

		"Venus/vendor/stb_image/**.h",
		"Venus/vendor/stb_image/**.cpp",
		"Venus/vendor/glm/glm/**.hpp",
		"Venus/vendor/glm/glm/**.inl",

		"Venus/vendor/ImGuizmo/ImGuizmo.h",
		"Venus/vendor/ImGuizmo/ImGuizmo.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{

		"%{prj.name}/src",
		"%{prj.name}/vendor",

		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Box2D}",
		"%{IncludeDir.mono}"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"yaml-cpp",
		"Box2D",
		"opengl32.lib",

		"%{Library.mono}"
	}

	filter "files:Venus/vendor/ImGuizmo/ImGuizmo.cpp"
	flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "VS_DEBUG"
		runtime "Debug"
		symbols "on"

		links
		{
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}

	filter "configurations:Release"
		defines "VS_RELEASE"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

	filter "configurations:Dist"
		defines "VS_DIST"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

-- Venus Scripting
project "VenusScripting"
	location "VenusScripting"
	kind "SharedLib"
	language "C#"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/src/**.cs", 
	}


-- VENUS EDITOR PROJECT
project "VenusEditor"
	location "VenusEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	links
	{
		"Venus"
	}

	files
	{
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.c", 
		"%{prj.name}/src/**.hpp", 
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"%{prj.name}/src",

		"Venus/vendor/spdlog/include",
		
		"Venus/src",
		"Venus/vendor",
		
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "VS_DEBUG"
		runtime "Debug"
		symbols "on"

		links
		{
			"%{Library.Assimp_Debug}"
		}

		postbuildcommands
		{
			"{COPYDIR} \"%{LibraryDir.VulkanSDK_DebugDLL}\" \"%{cfg.targetdir}\"",
			'{COPY} "%{Binaries.Assimp_Debug}" "%{cfg.targetdir}"',
			'{COPY} "../Venus/vendor/mono/bin/Debug/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}

	filter "configurations:Release"
		defines "VS_RELEASE"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.Assimp_Release}"
		}

		postbuildcommands 
		{
			'{COPY} "%{Binaries.Assimp_Release}" "%{cfg.targetdir}"',
			'{COPY} "../Venus/vendor/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}

	filter "configurations:Dist"
		defines "VS_DIST"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.Assimp_Release}"
		}

		postbuildcommands 
		{
			'{COPY} "%{Binaries.Assimp_Release}" "%{cfg.targetdir}"',
			'{COPY} "../Venus/vendor/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}


-- SANDBOX PROJECT
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	links
	{
		"Venus"
	}

	files
	{
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.c", 
		"%{prj.name}/src/**.hpp", 
		"%{prj.name}/src/**.cpp" 
	}

	includedirs
	{
		"Venus/vendor/spdlog/include",

		"%{prj.name}/src",
		"Venus/src",
		"Venus/vendor",
		
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "VS_DEBUG"
		runtime "Debug"
		symbols "on"

		links
		{
			"%{Library.Assimp_Debug}"
		}

		postbuildcommands
		{
			"{COPYDIR} \"%{LibraryDir.VulkanSDK_DebugDLL}\" \"%{cfg.targetdir}\"",
			'{COPY} "%{Binaries.Assimp_Debug}" "%{cfg.targetdir}"',
			'{COPY} "../Venus/vendor/mono/bin/Debug/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}

	filter "configurations:Release"
		defines "VS_RELEASE"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.Assimp_Release}"
		}

		postbuildcommands 
		{
			'{COPY} "%{Binaries.Assimp_Release}" "%{cfg.targetdir}"',
			'{COPY} "../Venus/vendor/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}

	filter "configurations:Dist"
		defines "VS_DIST"
		runtime "Release"
		optimize "on"

		links
		{
			"%{Library.Assimp_Release}"
		}

		postbuildcommands 
		{
			'{COPY} "%{Binaries.Assimp_Release}" "%{cfg.targetdir}"',
			'{COPY} "../Venus/vendor/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}
