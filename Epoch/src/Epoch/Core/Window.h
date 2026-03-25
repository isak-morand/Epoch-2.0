#pragma once
#include <functional>
#include <memory>
#include <string>

namespace Epoch
{
	class Event;

	struct WindowProperties
	{
		std::string Title = "Epoch Application";
		uint32_t Width = 1280;
		uint32_t Height = 720;
	};

	class Window
	{
		using EventCallbackFn = std::function<void(Event&)>;
	public:
		virtual ~Window() = default;

		static std::unique_ptr<Window> Create(const WindowProperties& aProps);

		void SetEventCallback(const EventCallbackFn& aCallback) { myData.EventCallback = aCallback; }

		virtual void PollEvents() = 0;

		uint32_t GetWidth() const { return myData.Width; }
		uint32_t GetHeight() const { return myData.Height; }

	protected:
		Window() = default;
		
	protected:
		struct WindowData
		{
			std::string Title = "";
			uint32_t Width = 0;
			uint32_t Height = 0;

			EventCallbackFn EventCallback;
		} myData;
	};
}
