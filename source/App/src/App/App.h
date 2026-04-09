#pragma once
#include <Epoch/Core/Application.h>

namespace Epoch
{
	class WindowCloseEvent;
}

class App : public Epoch::Application
{
public:
	App();
	~App() override;

	void OnStart() override;
	void OnShutdown() override;

	void OnUpdate(float aDeltaTime) override;

	void OnEvent(Epoch::Event& aEvent) override;

private:
	bool OnWindowClose(Epoch::WindowCloseEvent& aEvent);

private:

};
