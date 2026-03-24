local GLFW_DIR = "%{wks.location}/vendor/glfw"

project "GLFW"
    kind "StaticLib"
    language "C"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        GLFW_DIR .. "/include/GLFW/glfw3.h",
        GLFW_DIR .. "/include/GLFW/glfw3native.h",

        GLFW_DIR .. "/src/context.c",
        GLFW_DIR .. "/src/init.c",
        GLFW_DIR .. "/src/input.c",
        GLFW_DIR .. "/src/monitor.c",
        GLFW_DIR .. "/src/platform.c",
        GLFW_DIR .. "/src/vulkan.c",
        GLFW_DIR .. "/src/window.c",
        
		GLFW_DIR .. "/src/null_init.c",
		GLFW_DIR .. "/src/null_joystick.c",
		GLFW_DIR .. "/src/null_monitor.c",
		GLFW_DIR .. "/src/null_window.c",
    }

    includedirs
    {
        GLFW_DIR .. "/include"
    }

    filter "system:windows"
        defines
        {
            "_GLFW_WIN32",
		    "_CRT_SECURE_NO_WARNINGS",
        }

        files
        {
            GLFW_DIR .. "/src/win32_init.c",
            GLFW_DIR .. "/src/win32_joystick.c",
            GLFW_DIR .. "/src/win32_module.c",
            GLFW_DIR .. "/src/win32_monitor.c",
            GLFW_DIR .. "/src/win32_time.c",
            GLFW_DIR .. "/src/win32_thread.c",
            GLFW_DIR .. "/src/win32_window.c",

            GLFW_DIR .. "/src/wgl_context.c",
            
			GLFW_DIR .. "/src/egl_context.c",
			GLFW_DIR .. "/src/osmesa_context.c"
        }

        --links
        --{
        --    "user32",
        --    "gdi32",
        --    "shell32"
        --}
    
