#pragma once
#include "Epoch/Core/Window.h"

struct GLFWwindow;

namespace Epoch::Desktop
{
	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowProperties& aProps);
		~GLFWWindow() override;

		void PollEvents() override;

	private:
		GLFWwindow* myWindow;
	};
}