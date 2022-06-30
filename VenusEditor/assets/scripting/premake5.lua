ProjectName = "Dev"
RootDirectory = "../../../"

workspace "%{ProjectName}"
	architecture "x64"
	targetdir "build"
	startproject "%{ProjectName}"
	
	configurations 
	{ 
		"Debug", 
		"Release",
		"Dist"
	}

local OutputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
group "Venus"
project "VenusScripting"
	location "%{RootDirectory}/VenusScripting"
	kind "SharedLib"
	language "C#"

	targetdir ("%{RootDirectory}VenusEditor/Resources/Scripting")
	objdir ("%{RootDirectory}VenusEditor/Resources/Scripting/Intermediates")

	files 
	{
		"%{RootDirectory}VenusScripting/src/**.cs", 
	}

group ""
project "%{ProjectName}"
	location "%{ProjectName}"
	kind "SharedLib"
	language "C#"

	targetname "%{ProjectName}"
	targetdir ("%{prj.location}/Binaries")
	objdir ("%{RootDirectory}VenusEditor/Resources/Scripting/Intermediates")

	files 
	{
		"**.cs", 
	}

	links
	{
		"VenusScripting"
	}