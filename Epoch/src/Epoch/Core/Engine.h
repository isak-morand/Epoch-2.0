#pragma once
#include <memory>
#include "Window.h"
#include "Epoch/Rendering/RendererDesc.h"

namespace Epoch
{
	class Application;
	class Window;
	class Event;
	class WindowCloseEvent;
	class WindowResizeEvent;
	class Renderer;

	struct EngineDesc
	{
		WindowDesc Window;
		RendererDesc Renderer;
	};

	class Engine
	{
	public:
		Engine() = delete;
		explicit Engine(const EngineDesc& aDesc);
		~Engine();

		static Engine* Get() { return staticInstance; }

		void SetApp(Application* aApp);

		void Run();
		void Stop() { myIsRunning = false; }

	private:
		void OnEvent(Event& aEvent);
		bool OnWindowClose(WindowCloseEvent& aEvent);
		bool OnWindowResize(WindowResizeEvent& aEvent);

	private:
		static inline Engine* staticInstance = nullptr;

		bool myIsRunning = false;

		Application* myApplication = nullptr;

		std::unique_ptr<Window> myWindow;
		std::unique_ptr<Renderer> myRenderer;
	};
}
