#include "epch.h"
#include "Window.h"

#ifdef EPOCH_PLATFORM_WINDOWS
#include "Epoch/Platform/GLFW/GLFWWindow.h"
#endif


namespace Epoch
{
	std::unique_ptr<Window> Epoch::Window::Create(const WindowDesc& aDesc)
	{
#ifdef EPOCH_PLATFORM_WINDOWS
		return std::make_unique<GLFW::GLFWWindow>(aDesc);
#else
#error Unknown platform
#endif
	}
}
