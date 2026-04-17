local TRACY_DIR = "%{wks.location}/vendor/tracy"

project "Tracy"
	kind "StaticLib"

	disablewarnings { "4244" }

    linkoptions { "/ignore:4006" }

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    defines
	{
		"TRACY_ENABLE",
		"TRACY_ON_DEMAND",
		"TRACY_CALLSTACK=10"
	}

	files
	{
		TRACY_DIR .. "/tracy/client/**.h",
		TRACY_DIR .. "/tracy/client/**.hpp",
		TRACY_DIR .. "/tracy/client/**.cpp",

		TRACY_DIR .. "/tracy/common/**.h",
		TRACY_DIR .. "/tracy/common/**.hpp",
		TRACY_DIR .. "/tracy/common/**.cpp",

		TRACY_DIR .. "/tracy/tracy/**.h",
		TRACY_DIR .. "/tracy/tracy/**.hpp",
		TRACY_DIR .. "/tracy/tracy/**.cpp",

		TRACY_DIR .. "/tracy/libbacktrace/alloc.cpp",
		TRACY_DIR .. "/tracy/libbacktrace/sort.cpp",
		TRACY_DIR .. "/tracy/libbacktrace/state.cpp",
	}

	includedirs { TRACY_DIR .. "/tracy/" }

	links
	{
		"DbgHelp",
		"ws2_32"
	}

	defines { "_CRT_SECURE_NO_WARNINGS" }
