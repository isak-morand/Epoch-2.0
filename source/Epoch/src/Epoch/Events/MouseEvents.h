#pragma once
#include "Event.h"
#include "Epoch/Input/KeyCodes.h"

namespace Epoch
{
	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) {}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Mouse)

	private:
		float m_XOffset, m_YOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		inline MouseButton GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Mouse | EventCategory::MouseButton)

	protected:
		MouseButtonEvent(MouseButton aButton) : m_Button(aButton) {}

		MouseButton m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(MouseButton aButton) : MouseButtonEvent(aButton) {}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(MouseButton aButton) : MouseButtonEvent(aButton) {}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}
