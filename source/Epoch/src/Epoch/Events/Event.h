#pragma once
#include <functional>

namespace Epoch
{
	enum class EventType
	{
		None,
		WindowClose, WindowMinimize, WindowResize,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseScrolled
	};

	enum class EventCategory
	{
		None = 0,
		Window = 1 << 0,
		Input = 1 << 1,
		Keyboard = 1 << 2,
		Mouse = 1 << 3,
		MouseButton = 1 << 4
	};

	inline EventCategory operator|(EventCategory a, EventCategory b)
	{
		return static_cast<EventCategory>(static_cast<int>(a) | static_cast<int>(b));
	}

#define EVENT_CLASS_TYPE(aType) static EventType GetStaticType() { return EventType::aType; }\
								EventType GetEventType() const override { return GetStaticType(); }

#define EVENT_CLASS_CATEGORY(aCategory) int GetCategoryFlags() const override { return static_cast<int>(aCategory); }

	class Event
	{
	public:
		Event() = default;
		virtual ~Event() = default;

		bool IsHandled() const { return m_IsHandled; }

		virtual EventType GetEventType() const = 0;
		virtual int GetCategoryFlags() const = 0;

		bool IsInCategory(EventCategory aCategory) const
		{
			return GetCategoryFlags() & (int)aCategory;
		}

	private:
		bool m_IsHandled = false;

		friend class EventDispatcher;
	};

#define BIND_EVENT_FN(fn) [this](auto& e) -> bool { return this->fn(e); }

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& aEvent) : m_Event(aEvent) {}

		template<typename T>
		bool Dispatch(EventFn<T> aFunc)
		{
			if (m_Event.GetEventType() != T::GetStaticType() || m_Event.m_IsHandled)
			{
				return false;
			}

			m_Event.m_IsHandled = aFunc(*(T*)&m_Event);
			return true;
		}
	private:
		Event& m_Event;
	};
}
