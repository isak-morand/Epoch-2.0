#include "epch.h"
#include "Window.h"

#ifdef PLATFORM_WINDOWS
#include "Epoch/Platform/Desktop/GLFWWindow.h"
#endif


namespace Epoch
{
	std::unique_ptr<Window> Epoch::Window::Create(const WindowProperties& aProps)
	{
#ifdef PLATFORM_WINDOWS
		return std::make_unique<Desktop::GLFWWindow>(aProps);
#else
#error Unknown platform
#endif
	}
}
