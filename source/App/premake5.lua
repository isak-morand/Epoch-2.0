project "App"
	kind "ConsoleApp"
	filter { "system:windows", "configurations:Dist" }
		kind "WindowedApp"
	filter {}
	
	targetname ("App")

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/source/Epoch/src",
		"%{wks.location}/source/CommonUtilities/src",

		"%{wks.location}/vendor/spdlog/include",
		"%{wks.location}/vendor/tracy/tracy",
		"%{wks.location}/vendor/yaml-cpp/include",
	}

	links
	{
		"Epoch",
	}
	
	defines { "_CRT_SECURE_NO_WARNINGS", "SPDLOG_USE_STD_FORMAT" }
	
	filter "configurations:Debug or configurations:Release"
		defines
		{
			"TRACY_ENABLE",
			"TRACY_ON_DEMAND",
			"TRACY_CALLSTACK=10"
		}
	