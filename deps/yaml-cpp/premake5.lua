local YAML_DIR = "%{wks.location}/vendor/yaml-cpp"

project "yaml-cpp"
	kind "StaticLib"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		YAML_DIR .. "/src/**.h",
		YAML_DIR .. "/src/**.cpp",

		YAML_DIR .. "/include/**.h"
	}

	includedirs
	{
		YAML_DIR .. "/include"
	}

	defines { "YAML_CPP_STATIC_DEFINE" }
	
	filter "files:**yaml-cpp/**.cpp"
    	warnings "Off"

