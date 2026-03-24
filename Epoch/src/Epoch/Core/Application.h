#pragma once
#include "Epoch/Events/WindowEvents.h"

namespace Epoch
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	protected:
		virtual void OnUpdate() = 0;
		virtual void OnEvent(Event& aEvent) = 0;

		bool OnWindowClose(const WindowCloseEvent& aEvent) { myIsRunning = false; return true; }
		bool OnWindowResize(const WindowResizeEvent& aEvent) { return true; }
		bool OnWindowMinimiz(const WindowMinimizeEvent& aEvent) { return true; }

	private:
		bool myIsRunning = false;
	};
}
