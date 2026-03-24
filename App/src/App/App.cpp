#include "App.h"
#include <Epoch/Interface/Window.h>
#include <Epoch/Events/WindowEvents.h>


App::App() : Application()
{
	myWindow = Epoch::Window::Create(Epoch::WindowProperties());
	myWindow->SetEventCallback([this](Epoch::Event& aEvent) { OnEvent(aEvent); });
}

App::~App()
{
}

void App::OnUpdate()
{
	myWindow->PollEvents();
}

void App::OnEvent(Epoch::Event& aEvent)
{
	Epoch::EventDispatcher dispatcher(aEvent);

	if (aEvent.IsInCategory(Epoch::EventCategory::Window))
	{
		dispatcher.Dispatch<Epoch::WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<Epoch::WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
		dispatcher.Dispatch<Epoch::WindowMinimizeEvent>(BIND_EVENT_FN(OnWindowMinimiz));
	}
	else if (aEvent.IsInCategory(Epoch::EventCategory::Input))
	{
		//Input::OnEvent(aEvent);
	}
}
