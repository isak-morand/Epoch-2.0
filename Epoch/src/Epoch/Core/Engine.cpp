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
		staticInstance = this;

		myWindow = Window::Create(aDesc.Window);
		myWindow->SetEventCallback([this](Event& aEvent) { OnEvent(aEvent); });

		myRenderer = std::make_unique<Renderer>(myWindow.get(), aDesc.Renderer);
	}

	Engine::~Engine()
	{
	}

	void Engine::SetApp(Application* aApp)
	{
		myApplication = aApp;
	}

	void Engine::Run()
	{
		if (myApplication)
		{
			myApplication->OnStart();
		}

		std::thread renderThread([this]() {
			while (myIsRunning)
			{
				EPOCH_PROFILE_SCOPE("Render Thread");
				myRenderer->Render();
			}
			});

		myIsRunning = true;
		while (myIsRunning)
		{
			EPOCH_PROFILE_SCOPE("Frame");

			myWindow->PollEvents();

			if (myApplication)
			{
				myApplication->OnUpdate(1);
			}

			EPOCH_PROFILE_MARK_FRAME;
		}

		renderThread.join();

		if (myApplication)
		{
			myApplication->OnShutdown();
		}
	}

	void Engine::OnEvent(Event& aEvent)
	{
		if (myApplication)
		{
			myApplication->OnEvent(aEvent);
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
		myIsRunning = false;
		return true;
	}

	bool Engine::OnWindowResize(WindowResizeEvent& aEvent)
	{
		myRenderer->OnWindowResize(aEvent.GetWidth(), aEvent.GetHeight());
		return true;
	}
}
