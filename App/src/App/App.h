#pragma once
#include <Epoch/Core/Application.h>

class App : public Epoch::Application
{
public:
	App();
	~App() override;

	void OnStart() override;
	void OnShutdown() override;

	void OnUpdate(float aDeltaTime) override;

private:

};
