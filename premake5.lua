--function apply_simd_flags()
--    filter { "configurations:Release or configurations:Dist" }
--        vectorextensions "AVX2"
--        isaextensions { "BMI", "POPCNT", "LZCNT", "F16C" }
--    filter {}
--end

workspace "Epoch"
	startproject "App"

	architecture "x64"
	
	language "C++"
	cppdialect "C++20"
	
	configurations { "Debug", "Release", "Dist" }
	systemversion "latest"
	
	multiprocessorcompile ("On")
	--enableunitybuild ("On")

	staticruntime "Off"

	filter "configurations:Debug"
    	runtime "Debug"

	filter "configurations:Release or configurations:Dist"
    	runtime "Release"

	filter "system:windows"
		defines { "PLATFORM_WINDOWS" }
		buildoptions { "/Zc:preprocessor", "/Zc:__cplusplus" }
		
	filter "configurations:Debug"
		defines { "_DEBUG" }
		optimize "Off"
		symbols "on"
		
	filter "configurations:Release"
		defines { "_RELEASE", "NDEBUG" }
		optimize "On"
		symbols "default"
		
	filter "configurations:Dist"
		defines { "_DIST", "NDEBUG" }
		optimize "Full"
		symbols "off"

	outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

	defines 
	{
		"_CRT_SECURE_NO_WARNINGS",
		"SPDLOG_USE_STD_FORMAT"
	}

	--filter "action:vs*"
    --    linkoptions { "/ignore:4099" }

	include "Epoch"
	include "App"

	group "Dependencies"
		--include "CommonUtilities"
		include "vendor/glfw"
	group ""
