#include "epch.h"
#include "Engine.h"
#include "Application.h"
#include "Epoch/Events/Event.h"
#include "Epoch/Events/WindowEvents.h"

namespace Epoch
{
	Engine::Engine(const EngineProperties& aProps)
	{
		myWindow = Window::Create(aProps.WindowProps);
		myWindow->SetEventCallback([this](Event& aEvent) { OnEvent(aEvent); });
	}

	Engine::~Engine()
	{
	}

	void Engine::Run(Application* aApp)
	{
		aApp->OnStart();

		myIsRunning = true;
		while (myIsRunning)
		{
			myWindow->PollEvents();
			aApp->OnUpdate(1);
		}

		aApp->OnShutdown();
	}

	void Engine::OnEvent(Event& aEvent)
	{
		EventDispatcher dispatcher(aEvent);

		if (aEvent.IsInCategory(EventCategory::Window))
		{
			dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
			//dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
			//dispatcher.Dispatch<WindowMinimizeEvent>(BIND_EVENT_FN(OnWindowMinimiz));
		}
		else if (aEvent.IsInCategory(EventCategory::Input))
		{
			//Input::OnEvent(aEvent);
		}
	}
}
