workspace "Epoch"
	startproject "App"

	configurations { "Debug", "Develop", "Release" }
	architecture "x64"
	
	language "C++"
	cppdialect "C++20"
	staticruntime "Off"
	
	multiprocessorcompile ("On")
	
-- ========================
-- Platform-specific
-- ========================

	filter "system:windows"
		defines { "EPOCH_PLATFORM_WINDOWS" }
		systemversion "latest"
    	buildoptions { "/Zc:preprocessor", "/Zc:__cplusplus" }
		
	filter "system:linux"
    	defines { "EPOCH_PLATFORM_LINUX" }
    	--pic "On"

-- ========================
-- Configurations
-- ========================

	filter "configurations:Debug"
		defines { "_DEBUG" }
    	runtime "Debug"
		optimize "Off"
		symbols "On"
		
	filter "configurations:Develop"
		defines { "_DEVELOP", "NDEBUG" }
    	runtime "Release"
		optimize "On"
		symbols "Default"
		
	filter "configurations:Release"
		defines { "_RELEASE", "NDEBUG" }
    	runtime "Release"
		optimize "Full"
		symbols "Off"
		
	filter {}

-- ========================
-- Projects
-- ========================

	outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

	include "source/Epoch"
	include "source/App"
	include "source/CommonUtilities"

	group "Dependencies"
		include "deps/glfw"
		include "vendor/tracy"
		include "deps/yaml-cpp"
	group ""

	group "Dependencies/NVRHI"
		include "deps/NVRHI"
	group ""
