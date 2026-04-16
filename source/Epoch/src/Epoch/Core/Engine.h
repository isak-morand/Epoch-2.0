#pragma once
#include <memory>
#include <CommonUtilities/Timer.h>
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

		Window* GetWindow() { return m_Window.get(); }
		Renderer* GetRenderer() { return m_Renderer.get(); }

		GraphicsAPI GetGraphicsAPI() const;

		void Run();
		void Stop() { m_IsRunning = false; }

		float GetDeltaTime() const { return m_DeltaTime; }
		float GetTotalTime() const { return m_Timer.Elapsed(); }
		uint32_t GetFrameCount() const { return m_FrameCount; }

	private:
		void OnEvent(Event& aEvent);
		bool OnWindowClose(WindowCloseEvent& aEvent);
		bool OnWindowResize(WindowResizeEvent& aEvent);

	private:
		static inline Engine* s_Instance = nullptr;

		Application* m_Application = nullptr;

		bool m_IsRunning = false;

		CU::Timer m_Timer;
		float m_DeltaTime = 0.0f;
		float m_LastTime = 0.0f;
		uint32_t m_FrameCount = 0u;

		std::unique_ptr<Window> m_Window;
		std::unique_ptr<Renderer> m_Renderer;
	};
}
