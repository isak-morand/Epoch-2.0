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
	staticruntime "Off"
	
	configurations { "Debug", "Release", "Dist" }
	systemversion "latest"
	
	multiprocessorcompile ("On")
	
	filter "configurations:Debug"
    	runtime "Debug"

	filter "configurations:Release or configurations:Dist"
    	runtime "Release"

	filter "system:windows"
		defines { "EPOCH_WIN32" }
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

	include "Epoch"
	include "App"

	group "Dependencies"
		--include "CommonUtilities"
		include "deps/glfw"
		include "vendor/tracy"
		include "deps/yaml-cpp"
	group ""

	group "Dependencies/NVRHI"
		include "deps/NVRHI"
	group ""
