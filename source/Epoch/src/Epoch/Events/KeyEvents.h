#pragma once
#include "Event.h"
#include "Epoch/Input/KeyCodes.h"

namespace Epoch
{
	class KeyEvent : public Event
	{
	public:
		inline KeyCode GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategory::Input | EventCategory::Keyboard)

	protected:
		KeyEvent(KeyCode aKeycode) : m_KeyCode(aKeycode) {}

		KeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(KeyCode aKeycode) : KeyEvent(aKeycode) {}

		EVENT_CLASS_TYPE(KeyPressed)
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(KeyCode aKeycode) : KeyEvent(aKeycode) {}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}