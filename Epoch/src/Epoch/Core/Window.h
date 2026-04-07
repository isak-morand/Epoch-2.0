#pragma once
#include <functional>
#include <memory>
#include <string>

namespace Epoch
{
	class Event;

	struct WindowDesc
	{
		std::string title = "Epoch Application";
		uint32_t width = 1280;
		uint32_t height = 720;

		bool startMaximized = false;
		bool startFullscreen = false;
		bool startBorderless = false;
	};

	class Window
	{
		using EventCallbackFn = std::function<void(Event&)>;
	public:
		virtual ~Window() = default;

		static std::unique_ptr<Window> Create(const WindowDesc& aDesc);

		void SetEventCallback(const EventCallbackFn& aCallback) { myData.eventCallback = aCallback; }

		uint32_t GetWidth() const { return myData.width; }
		uint32_t GetHeight() const { return myData.height; }

		virtual void PollEvents() = 0;

		virtual void* GetNativeWindow() const = 0;

	protected:
		Window() = default;
		
	protected:
		struct WindowData
		{
			std::string title = "";
			uint32_t width = 0;
			uint32_t height = 0;

			EventCallbackFn eventCallback;
		} myData;
	};
}
