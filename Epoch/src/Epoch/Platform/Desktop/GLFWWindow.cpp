#include "epch.h"
#include "GLFWWindow.h"

#include <GLFW/glfw3.h>
#ifdef EPOCH_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>
#endif

#include "Epoch/Events/WindowEvents.h"

namespace Epoch::Desktop
{
	#define LOG_TAG LogTags::Core

	static void GLFWErrorCallback(int aError, const char* aDescription)
	{
		LOG_ERROR(LOG_TAG, "GLFW Error ({}): {}", aError, aDescription);
	}

	GLFWWindow::GLFWWindow(const WindowDesc& aDesc)
	{
		myData.Width = aDesc.Width;
		myData.Height = aDesc.Height;
		myData.Title = aDesc.Title;

		EPOCH_VERIFY(glfwInit(), "Could not initialize GLFW!");

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
		//glfwWindowHint(GLFW_VISIBLE, aProps.StartHidden ? GLFW_FALSE : GLFW_TRUE);

		myWindow = glfwCreateWindow
		(
			(int)myData.Width,
			(int)myData.Height,
			myData.Title.c_str(),
			nullptr,
			nullptr
		);

		EPOCH_VERIFY(myWindow, "Failed to create GLFW window!");

		glfwSetWindowUserPointer(myWindow, &myData);

		if (glfwRawMouseMotionSupported())
		{
			glfwSetInputMode(myWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}
		else
		{
			LOG_WARNING(LOG_TAG, "Raw mouse motion not supported.");
		}

		//Callbacks
		{
			glfwSetWindowCloseCallback(myWindow, [](GLFWwindow* window)
				{
					auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

					WindowCloseEvent event;
					data.EventCallback(event);
				});

			glfwSetWindowSizeCallback(myWindow, [](GLFWwindow* window, int width, int height)
				{
					auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

					WindowResizeEvent event((uint32_t)width, (uint32_t)height);
					data.Width = width;
					data.Height = height;
					data.EventCallback(event);
				});

			glfwSetWindowIconifyCallback(myWindow, [](GLFWwindow* window, int iconified)
				{
					auto& data = *((WindowData*)glfwGetWindowUserPointer(window));
					WindowMinimizeEvent event((bool)iconified);
					data.EventCallback(event);
				});

			glfwSetKeyCallback(myWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
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

			glfwSetCharCallback(myWindow, [](GLFWwindow* window, uint32_t codepoint)
				{
					//auto& data = *((WindowData*)glfwGetWindowUserPointer(window));
					//
					//KeyTypedEvent event((KeyCode)codepoint);
					//data.EventCallback(event);
				});

			glfwSetMouseButtonCallback(myWindow, [](GLFWwindow* window, int button, int action, int mods)
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

			glfwSetScrollCallback(myWindow, [](GLFWwindow* window, double xOffset, double yOffset)
				{
					//auto& data = *((WindowData*)glfwGetWindowUserPointer(window));
					//
					//MouseScrolledEvent event((float)xOffset, (float)yOffset);
					//data.EventCallback(event);
				});

			glfwSetCursorPosCallback(myWindow, [](GLFWwindow* window, double x, double y)
				{
					//auto& data = *((WindowData*)glfwGetWindowUserPointer(window));
					//
					//MouseMovedEvent event((float)x, (float)y);
					//data.EventCallback(event);
				});
		}

		int width, height;
		glfwGetWindowSize(myWindow, &width, &height);
		myData.Width = width;
		myData.Height = height;

		LOG_INFO(LOG_TAG, "Window created: {} ({}, {})", myData.Title, myData.Width, myData.Height);
	}

	GLFWWindow::~GLFWWindow()
	{
		glfwDestroyWindow(myWindow);
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
			glfwSetWindowTitle(myWindow, title.c_str());

			totalTime = 0.0;
			frames = 0;
		}

		glfwPollEvents();
	}

	void* GLFWWindow::GetNativeWindow() const
	{
#ifdef EPOCH_WIN32
		return (void*)glfwGetWin32Window(myWindow);
#else
		return nullptr;
#endif
	}
}
