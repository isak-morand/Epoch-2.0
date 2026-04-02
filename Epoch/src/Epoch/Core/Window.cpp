#include "epch.h"
#include "Window.h"

#ifdef EPOCH_WIN32
#include "Epoch/Platform/Desktop/GLFWWindow.h"
#endif


namespace Epoch
{
	std::unique_ptr<Window> Epoch::Window::Create(const WindowDesc& aDesc)
	{
#ifdef EPOCH_WIN32
		return std::make_unique<Desktop::GLFWWindow>(aDesc);
#else
#error Unknown platform
#endif
	}
}
