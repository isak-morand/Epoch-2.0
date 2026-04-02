#pragma once
#include <functional>
#include <memory>
#include <string>

namespace Epoch
{
	class Event;

	struct WindowDesc
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

		static std::unique_ptr<Window> Create(const WindowDesc& aDesc);

		void SetEventCallback(const EventCallbackFn& aCallback) { myData.EventCallback = aCallback; }

		uint32_t GetWidth() const { return myData.Width; }
		uint32_t GetHeight() const { return myData.Height; }

		virtual void PollEvents() = 0;

		virtual void* GetNativeWindow() const = 0;

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
