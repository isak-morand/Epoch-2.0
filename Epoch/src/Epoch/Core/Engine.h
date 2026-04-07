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
		WindowDesc window;
		RendererDesc renderer;
	};

	class Engine
	{
	public:
		Engine() = delete;
		explicit Engine(const EngineDesc& aDesc);
		~Engine();

		static Engine* Get() { return s_Instance; }

		void SetApp(Application* aApp);

		void Run();
		void Stop() { m_IsRunning = false; }

	private:
		void OnEvent(Event& aEvent);
		bool OnWindowClose(WindowCloseEvent& aEvent);
		bool OnWindowResize(WindowResizeEvent& aEvent);

	private:
		static inline Engine* s_Instance = nullptr;

		bool m_IsRunning = false;

		Application* m_Application = nullptr;

		std::unique_ptr<Window> m_Window;
		std::unique_ptr<Renderer> m_Renderer;
	};
}
