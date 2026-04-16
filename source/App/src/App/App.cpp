#include "App.h"
#include <Epoch/Debug/Log.h>
#include <Epoch/Events/WindowEvents.h>
#include <Epoch/Core/Engine.h>

App::App() {}
App::~App() {}

void App::OnStart() {}
void App::OnShutdown() {}

void App::OnUpdate(float aDeltaTime)
{
	static float totalTime = 0.0f;
	totalTime += aDeltaTime;

	if (totalTime >= 1.f)
	{
		const Epoch::EngineStats& stats = Epoch::Engine::Get()->GetEngineStats();
		std::string title = std::format("Epoch <{}> {} updates, {} fps",
			Epoch::RHI::APIToString(Epoch::Engine::Get()->GetGraphicsAPI()),
			(uint32_t)(1.f / stats.gameFrameTimeAvg),
			(uint32_t)(1.f / stats.renderFrameTimeAvg));
		UpdateWindowTitle(title);

		totalTime = 0.0f;
	}
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

void App::UpdateWindowTitle(const std::string& aTitle)
{
	Epoch::Engine::Get()->GetWindow()->SetTitle(aTitle);
}
