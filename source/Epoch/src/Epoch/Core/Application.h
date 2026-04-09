#pragma once

namespace Epoch
{
	class Event;

	class Application
	{
	public:
		Application() = default;
		virtual ~Application() = default;

		virtual void OnStart() = 0;
		virtual void OnShutdown() = 0;

		virtual void OnUpdate(float aDeltaTime) = 0;

		virtual void OnEvent(Event& aEvent) {}
	};
}