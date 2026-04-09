#pragma once
#include "Event.h"

namespace Epoch
{
	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategory::Window)
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int aWidth, unsigned int aHeight) : m_Width(aWidth), m_Height(aHeight) {}

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategory::Window)

	private:
		unsigned int m_Width, m_Height;
	};

	class WindowMinimizeEvent : public Event
	{
	public:
		WindowMinimizeEvent(bool minimized) : m_Minimized(minimized) {}

		bool IsMinimized() const { return m_Minimized; }

		EVENT_CLASS_TYPE(WindowMinimize)
		EVENT_CLASS_CATEGORY(EventCategory::Window)

	private:
		bool m_Minimized = false;
	};
}
