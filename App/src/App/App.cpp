#include "App.h"
#include <Epoch/Events/WindowEvents.h>
#include <Epoch/Core/Engine.h>

App::App()
{
}

App::~App()
{
}

void App::OnStart()
{
}

void App::OnShutdown()
{
}

void App::OnUpdate(float aDeltaTime)
{
}

void App::OnEvent(Epoch::Event& aEvent)
{
	Epoch::EventDispatcher dispatcher(aEvent);
	dispatcher.Dispatch<Epoch::WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
}

bool App::OnWindowClose(Epoch::WindowCloseEvent& aEvent)
{
	Epoch::Engine::Get()->Stop();
	return true;
}
