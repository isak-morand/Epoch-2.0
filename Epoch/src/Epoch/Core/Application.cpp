#include "epch.h"
#include "Application.h"

namespace Epoch
{
	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		myIsRunning = true;

		while (myIsRunning)
		{
			OnUpdate();
		}
	}
}
