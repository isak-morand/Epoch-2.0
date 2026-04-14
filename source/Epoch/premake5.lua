project "Epoch"
	kind "StaticLib"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "epch.h"
	pchsource "src/epch.cpp"

	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/source/CommonUtilities/src",

		"%{wks.location}/vendor/spdlog/include",
		"%{wks.location}/vendor/glfw/include",
		"%{wks.location}/vendor/tracy/tracy",
		"%{wks.location}/vendor/yaml-cpp/include",

		"%{wks.location}/vendor/NVRHI/include",
		"%{wks.location}/vendor/NVRHI/thirdparty/DirectX-Headers/include",
		"%{wks.location}/vendor/NVRHI/thirdparty/Vulkan-Headers/include",
	}

	links
	{
		"CommonUtilities",

		"GLFW",
		"NVRHI",
		"Tracy",
		"yaml-cpp",
    }

	filter "configurations:Debug or configurations:Release"
		defines
		{
			"TRACY_ENABLE",
			"TRACY_ON_DEMAND",
			"TRACY_CALLSTACK=10"
		}

	filter "configurations:Debug"
		defines { "EPOCH_DEBUG" }

	filter "configurations:Release"
		defines { "EPOCH_RELEASE" }

	filter "configurations:Dist"
		defines { "EPOCH_DIST" }

	filter "system:windows"
		defines
		{
			"WIN32_LEAN_AND_MEAN",
			"NOMINMAX",
			"GLFW_INCLUDE_NONE",
		}
		
	defines { "_CRT_SECURE_NO_WARNINGS", "SPDLOG_USE_STD_FORMAT", "YAML_CPP_STATIC_DEFINE" }

	filter "system:windows"
    	includedirs
    	{
    	    "%{wks.location}/vendor/dxc/windows/include",
    	}

    	libdirs
    	{
    	    "%{wks.location}/vendor/dxc/windows/lib",
    	}

    	links
    	{
    	    "dxcompiler"
    	}

	filter "system:linux"
    	includedirs
    	{
    	    "%{wks.location}/vendor/dxc/linux/include",
    	}

    	libdirs
    	{
    	    "%{wks.location}/vendor/dxc/linux/lib",
    	}

    	links
    	{
    	    "dxcompiler" -- links libdxcompiler.so
    	}
