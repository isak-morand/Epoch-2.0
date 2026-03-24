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
		--"%{wks.location}/CommonUtilities/src",
		"%{wks.location}/vendor/glfw/include",
    }

    links
	{
        --"CommonUtilities",
		"GLFW",
    }

	--filter "configurations:Debug or configurations:Release"
	--	defines
	--	{
	--		"TRACY_ENABLE",
	--		"TRACY_ON_DEMAND",
	--		"TRACY_CALLSTACK=10"
	--	}

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
