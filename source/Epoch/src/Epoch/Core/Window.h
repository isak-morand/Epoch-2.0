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

		void SetEventCallback(const EventCallbackFn& aCallback) { m_EventCallback = aCallback; }

		std::string_view GetTitle() const { return m_Title; }

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		bool IsMinimized() const { return myIsMinimized; }

		virtual void PollEvents() = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual void SetTitle(const std::string& aTitle) = 0;

	protected:
		Window() = default;
		
	protected:
		std::string m_Title = "";
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		bool myIsMinimized = false;

		EventCallbackFn m_EventCallback;
	};
}
