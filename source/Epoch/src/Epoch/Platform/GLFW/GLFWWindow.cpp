#include "epch.h"
#include "GLFWWindow.h"

#include <GLFW/glfw3.h>
#ifdef EPOCH_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#endif

#include "Epoch/Events/WindowEvents.h"
#include "Epoch/Events/MouseEvents.h"
#include "Epoch/Events/KeyEvents.h"

namespace Epoch::GLFW
{
	#define LOG_TAG LogTags::Core

	namespace
	{
		void ErrorCallback_GLFW(int aError, const char* aDescription)
		{
			LOG_ERROR(LOG_TAG, "GLFW Error ({}): {}", aError, aDescription);
		}

		void WindowCloseCallback_GLFW(GLFWwindow* window)
		{
			GLFWWindow* win = (GLFWWindow*)glfwGetWindowUserPointer(window);
			win->WindowCloseCallback();
		}

		void WindowSizeCallback_GLFW(GLFWwindow* window, int width, int height)
		{
			GLFWWindow* win = (GLFWWindow*)glfwGetWindowUserPointer(window);
			win->WindowSizeCallback(width, height);
		}

		void WindowIconifyCallback_GLFW(GLFWwindow* window, int iconified)
		{
			GLFWWindow* win = (GLFWWindow*)glfwGetWindowUserPointer(window);
			win->WindowIconifyCallback(iconified);
		}

		static void KeyCallback_GLFW(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			GLFWWindow* win = (GLFWWindow*)glfwGetWindowUserPointer(window);
			win->KeyCallback(key, scancode, action, mods);
		}

		static void MouseButtonCallback_GLFW(GLFWwindow* window, int button, int action, int mods)
		{
			GLFWWindow* win = (GLFWWindow*)glfwGetWindowUserPointer(window);
			win->MouseButtonCallback(button, action, mods);
		}

		static void ScrollCallback_GLFW(GLFWwindow* window, double xoffset, double yoffset)
		{
			GLFWWindow* win = (GLFWWindow*)glfwGetWindowUserPointer(window);
			win->ScrollCallback(xoffset, yoffset);
		}
	}

	GLFWWindow::GLFWWindow(const WindowDesc& aDesc)
	{
		m_Width = aDesc.width;
		m_Height = aDesc.height;
		m_Title = aDesc.title;

		EPOCH_VERIFY(glfwInit(), "Could not initialize GLFW!");

		glfwSetErrorCallback(ErrorCallback_GLFW);

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		if (aDesc.startBorderless)
		{
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		}

		m_Window = glfwCreateWindow
		(
			(int)m_Width,
			(int)m_Height,
			m_Title.c_str(),
			nullptr,
			nullptr
		);

		EPOCH_VERIFY(m_Window, "Failed to create GLFW window!");

		if (aDesc.startFullscreen)
		{
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			int x, y;
			glfwGetMonitorPos(monitor, &x, &y);

			glfwSetWindowMonitor(m_Window, nullptr, x, y, mode->width, mode->height, mode->refreshRate);
		}

		if (aDesc.startMaximized)
		{
			glfwMaximizeWindow(m_Window);
		}

		int width, height;
		glfwGetWindowSize(m_Window, &width, &height);
		m_Width = width;
		m_Height = height;

		glfwShowWindow(m_Window);

		if (glfwRawMouseMotionSupported())
		{
			glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
		else
		{
			LOG_WARN(LOG_TAG, "Raw mouse motion not supported.");
		}

		glfwSetWindowUserPointer(m_Window, this);

		glfwSetWindowCloseCallback(m_Window, WindowCloseCallback_GLFW);
		glfwSetWindowSizeCallback(m_Window, WindowSizeCallback_GLFW);
		glfwSetWindowIconifyCallback(m_Window, WindowIconifyCallback_GLFW);
		glfwSetKeyCallback(m_Window, KeyCallback_GLFW);
		glfwSetMouseButtonCallback(m_Window, MouseButtonCallback_GLFW);
		glfwSetScrollCallback(m_Window, ScrollCallback_GLFW);

		LOG_INFO(LOG_TAG, "Window created: {} ({}, {})", m_Title, m_Width, m_Height);
	}

	GLFWWindow::~GLFWWindow()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void GLFWWindow::PollEvents()
	{
		glfwPollEvents();
	}

	void* GLFWWindow::GetNativeWindow() const
	{
#ifdef EPOCH_PLATFORM_WINDOWS
		return (void*)glfwGetWin32Window(m_Window);
#else
		return nullptr;
#endif
	}

	void GLFWWindow::SetTitle(const std::string& aTitle)
	{
		glfwSetWindowTitle(m_Window, aTitle.c_str());
	}

	void GLFWWindow::WindowCloseCallback()
	{
		WindowCloseEvent event;
		m_EventCallback(event);
	}

	void GLFWWindow::WindowSizeCallback(int aWidth, int aHeight)
	{
		m_Width = aWidth;
		m_Height = aHeight;
		WindowResizeEvent event((uint32_t)aWidth, (uint32_t)aHeight);
		m_EventCallback(event);
	}

	void GLFWWindow::WindowIconifyCallback(int aIconified)
	{
		myIsMinimized = (bool)aIconified;
		WindowMinimizeEvent event(myIsMinimized);
		m_EventCallback(event);
	}

	void GLFWWindow::KeyCallback(int aKey, int aScancode, int aAction, int aMods)
	{
		switch (aAction)
		{
		case GLFW_PRESS:
		{
			KeyPressedEvent event((KeyCode)aKey);
			m_EventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			KeyReleasedEvent event((KeyCode)aKey);
			m_EventCallback(event);
			break;
		}
		}
	}

	void GLFWWindow::MouseButtonCallback(int aButton, int aAction, int aMods)
	{
		switch (aAction)
		{
		case GLFW_PRESS:
		{
			MouseButtonPressedEvent event((MouseButton)aButton);
			m_EventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			MouseButtonReleasedEvent event((MouseButton)aButton);
			m_EventCallback(event);
			break;
		}
		}
	}

	void GLFWWindow::ScrollCallback(double aXOffset, double aYOffset)
	{
		MouseScrolledEvent event((float)aXOffset, (float)aYOffset);
		m_EventCallback(event);
	}
}
