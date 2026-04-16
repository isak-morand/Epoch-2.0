#include "epch.h"
#include "Engine.h"
#include "Application.h"
#include "Epoch/Events/Event.h"
#include "Epoch/Events/WindowEvents.h"
#include "Epoch/Rendering/Renderer.h"

#include <thread>
#include "Epoch/Debug/Profiler.h"

namespace Epoch
{
	Engine::Engine(const EngineDesc& aDesc)
	{
		EPOCH_ASSERT(!s_Instance, "Engine already exists!");
		s_Instance = this;

		m_Window = Window::Create(aDesc.window);
		m_Window->SetEventCallback([this](Event& aEvent) { OnEvent(aEvent); });

		m_Renderer = std::make_unique<Renderer>(m_Window.get(), aDesc.renderer);
	}

	Engine::~Engine()
	{
		s_Instance = nullptr;
	}

	void Engine::SetApp(Application* aApp)
	{
		m_Application = aApp;
	}

	RHI::API Engine::GetGraphicsAPI() const
	{
		return m_Renderer->GetAPI();
	}

	void Engine::Run()
	{
		if (m_Application)
		{
			m_Application->OnStart();
		}

		std::thread renderThread([this]()
			{
				while (m_IsRunning)
				{
					if (m_Window->IsMinimized()) continue;

					EPOCH_PROFILE_SCOPE("Render Thread");
					m_Renderer->Render();
				}
			});

		m_Timer.Reset();
		m_IsRunning = true;
		while (m_IsRunning)
		{
			EPOCH_PROFILE_SCOPE("Frame");

			m_Window->PollEvents();

			if (m_Application)
			{
				m_Application->OnUpdate(m_DeltaTime);
			}

			UpdateTime();
			UpdateEngineStats();

			EPOCH_PROFILE_MARK_FRAME;
		}

		renderThread.join();

		if (m_Application)
		{
			m_Application->OnShutdown();
		}
	}

	void Engine::OnEvent(Event& aEvent)
	{
		if (m_Application)
		{
			m_Application->OnEvent(aEvent);
		}

		if (aEvent.IsHandled()) return;

		EventDispatcher dispatcher(aEvent);

		if (aEvent.IsInCategory(EventCategory::Window))
		{
			dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
			dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
			//dispatcher.Dispatch<WindowMinimizeEvent>(BIND_EVENT_FN(OnWindowMinimiz));
		}
		else if (aEvent.IsInCategory(EventCategory::Input))
		{
			//Input::OnEvent(aEvent);
		}
	}

	bool Engine::OnWindowClose(WindowCloseEvent& aEvent)
	{
		m_IsRunning = false;
		return true;
	}

	bool Engine::OnWindowResize(WindowResizeEvent& aEvent)
	{
		m_Renderer->OnWindowResize(aEvent.GetWidth(), aEvent.GetHeight());
		return true;
	}

	void Engine::UpdateTime()
	{
		float time = m_Timer.Elapsed();
		m_DeltaTime = time - m_LastTime;
		m_LastTime = time;
		++m_FrameCount;

		m_FrameTimeSum += m_DeltaTime;
		++m_NumberOfAccumulatedFrames;

		if (m_FrameTimeSum > m_AverageTimeUpdateInterval && m_NumberOfAccumulatedFrames > 0)
		{
			m_AverageFrameTime = m_FrameTimeSum / m_NumberOfAccumulatedFrames;
			m_NumberOfAccumulatedFrames = 0;
			m_FrameTimeSum = 0.0;
		}
	}

	void Engine::UpdateEngineStats()
	{
		m_EngineStats.gameFrameTimeAvg = m_AverageFrameTime;
		m_EngineStats.renderFrameTimeAvg = m_Renderer->GetAverageFrameTime();
	}
}
