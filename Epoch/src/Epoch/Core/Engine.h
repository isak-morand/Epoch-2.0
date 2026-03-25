#pragma once
#include <memory>
#include "Window.h"

namespace Epoch
{
	class Application;
	class Window;
	class Event;
	class WindowCloseEvent;

	struct EngineProperties
	{
		WindowProperties WindowProps;
	};

	class Engine
	{
	public:
		Engine() = delete;
		explicit Engine(const EngineProperties& aProps);
		~Engine();

		void Run(Application* aApp);

	private:
		void OnEvent(Event& aEvent);
		bool OnWindowClose(WindowCloseEvent& aEvent) { myIsRunning = false; return true; }

	private:
		bool myIsRunning = false;

		std::unique_ptr<Window> myWindow;
	};
}
