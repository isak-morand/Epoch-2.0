#include "App.h"
#include <Epoch/Debug/Log.h>
#include <Epoch/Events/WindowEvents.h>
#include <Epoch/Core/Engine.h>
#include <Epoch/Rendering/Renderer.h>

#include <chrono>

App::App() {}
App::~App() {}

void App::OnStart() {}
void App::OnShutdown() {}

void App::OnUpdate(float aDeltaTime)
{
	static uint32_t frames = 0;
	static float totalTime = 0.5f;
	static std::chrono::time_point<std::chrono::high_resolution_clock> lastTime = std::chrono::high_resolution_clock::now();

	std::chrono::time_point<std::chrono::high_resolution_clock> time = std::chrono::high_resolution_clock::now();
	float delta = std::chrono::duration_cast<std::chrono::microseconds>(time - lastTime).count() * 0.001f * 0.001f;
	lastTime = time;

	frames++;
	totalTime += delta;

	if (totalTime >= 0.5)
	{
		uint32_t cpuFPS = (uint32_t)(frames / totalTime);

		float gpuFrameTime = Epoch::Engine::Get()->GetRenderer()->GetAverageFrameTime();
		uint32_t gpuFPS = (uint32_t)(1.0f / gpuFrameTime);

		std::string title = std::format("Epoch <{}> {} cpu, {} gpu", "d3d12", cpuFPS, gpuFPS);
		UpdateWindowTitle(title);

		totalTime -= 0.5f;
		frames = 0;
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
