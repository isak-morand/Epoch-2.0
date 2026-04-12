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
		s_Instance = this;

		m_Window = Window::Create(aDesc.window);
		m_Window->SetEventCallback([this](Event& aEvent) { OnEvent(aEvent); });

		m_Renderer = std::make_unique<Renderer>(m_Window.get(), aDesc.renderer);
	}

	Engine::~Engine()
	{
	}

	void Engine::SetApp(Application* aApp)
	{
		m_Application = aApp;
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

		m_IsRunning = true;
		while (m_IsRunning)
		{
			EPOCH_PROFILE_SCOPE("Frame");

			m_Window->PollEvents();

			if (m_Application)
			{
				m_Application->OnUpdate(1);
			}

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
}
