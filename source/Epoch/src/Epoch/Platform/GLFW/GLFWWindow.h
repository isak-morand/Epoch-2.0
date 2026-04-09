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

		void SetTitle(const std::string& aTitle) override;

		void WindowCloseCallback();
		void WindowSizeCallback(int aWidth, int aHeight);
		void WindowIconifyCallback(int aIconified);
		void KeyCallback(int aKey, int aScancode, int aAction, int aMods);
		void MouseButtonCallback(int aButton, int aAction, int aMods);
		void ScrollCallback(double aXOffset, double aYOffset);

	private:
		GLFWwindow* m_Window;
	};
}