project "App"
	kind "WindowedApp"
	entrypoint "mainCRTStartup"
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
		"%{wks.location}/Epoch/src",
    }

    links
	{
        --"CommonUtilities",
		"Epoch",
    }

	--filter "configurations:Debug or configurations:Release"
	--	defines
	--	{
	--		"TRACY_ENABLE",
	--		"TRACY_ON_DEMAND",
	--		"TRACY_CALLSTACK=10"
	--	}
