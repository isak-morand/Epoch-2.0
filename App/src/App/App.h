#pragma once
#include <memory>
#include <Epoch/Core/Application.h>

namespace Epoch
{
	class Window;
	class Event;
}

class App : public Epoch::Application
{
public:
	App();
	~App() override;

protected:
	void OnUpdate() override;
	void OnEvent(Epoch::Event& aEvent) override;

private:
	std::unique_ptr<Epoch::Window> myWindow;
};