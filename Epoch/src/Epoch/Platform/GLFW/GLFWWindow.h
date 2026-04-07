#pragma once
#include "Epoch/Core/Window.h"

struct GLFWwindow;

namespace Epoch::GLFW
{
	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowDesc& aDesc);
		~GLFWWindow() override;

		void PollEvents() override;

		void* GetNativeWindow() const override;

	private:
		GLFWwindow* myWindow;
	};
}