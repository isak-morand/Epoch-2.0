#include "epch.h"
#include "GLFWWindow.h"

#include <GLFW/glfw3.h>
#ifdef EPOCH_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#endif

#include "Epoch/Events/WindowEvents.h"

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
		//glfwWindowHint(GLFW_VISIBLE, aProps.StartHidden ? GLFW_FALSE : GLFW_TRUE);

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

		glfwSetWindowUserPointer(m_Window, this);

		if (glfwRawMouseMotionSupported())
		{
			glfwSetInputMode(m_Window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
		else
		{
			LOG_WARNING(LOG_TAG, "Raw mouse motion not supported.");
		}

		//Callbacks
		{
			glfwSetWindowCloseCallback(m_Window, WindowCloseCallback_GLFW);
			glfwSetWindowSizeCallback(m_Window, WindowSizeCallback_GLFW);
			glfwSetWindowIconifyCallback(m_Window, WindowIconifyCallback_GLFW);

			glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
				{
					//auto& data = *((WindowData*)glfwGetWindowUserPointer(window));
					//
					//switch (action)
					//{
					//case GLFW_PRESS:
					//{
					//	KeyPressedEvent event((KeyCode)key);
					//	data.EventCallback(event);
					//	break;
					//}
					//case GLFW_RELEASE:
					//{
					//	KeyReleasedEvent event((KeyCode)key);
					//	data.EventCallback(event);
					//	break;
					//}
					//}
				});

			glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32_t codepoint)
				{
					//auto& data = *((WindowData*)glfwGetWindowUserPointer(window));
					//
					//KeyTypedEvent event((KeyCode)codepoint);
					//data.EventCallback(event);
				});

			glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
				{
					//auto& data = *((WindowData*)glfwGetWindowUserPointer(window));
					//
					//switch (action)
					//{
					//case GLFW_PRESS:
					//{
					//	MouseButtonPressedEvent event((MouseButton)button);
					//	data.EventCallback(event);
					//	break;
					//}
					//case GLFW_RELEASE:
					//{
					//	MouseButtonReleasedEvent event((MouseButton)button);
					//	data.EventCallback(event);
					//	break;
					//}
					//}
				});

			glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
				{
					//auto& data = *((WindowData*)glfwGetWindowUserPointer(window));
					//
					//MouseScrolledEvent event((float)xOffset, (float)yOffset);
					//data.EventCallback(event);
				});

			glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y)
				{
					//auto& data = *((WindowData*)glfwGetWindowUserPointer(window));
					//
					//MouseMovedEvent event((float)x, (float)y);
					//data.EventCallback(event);
				});
		}

		LOG_INFO(LOG_TAG, "Window created: {} ({}, {})", m_Title, m_Width, m_Height);
	}

	GLFWWindow::~GLFWWindow()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void GLFWWindow::PollEvents()
	{
		static uint32_t frames = 0;
		static double totalTime = 0.0;
		static double lastTime = glfwGetTime();

		double time = glfwGetTime();
		double delta = time - lastTime;
		lastTime = time;

		frames++;
		totalTime += delta;

		if (totalTime >= 0.5)
		{
			double fps = frames / totalTime;

			std::string title = "FPS: " + std::to_string((int)fps);
			glfwSetWindowTitle(m_Window, title.c_str());

			totalTime = 0.0;
			frames = 0;
		}

		glfwPollEvents();
	}

	void* GLFWWindow::GetNativeWindow() const
	{
#ifdef EPOCH_WIN32
		return (void*)glfwGetWin32Window(m_Window);
#else
		return nullptr;
#endif
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
}
