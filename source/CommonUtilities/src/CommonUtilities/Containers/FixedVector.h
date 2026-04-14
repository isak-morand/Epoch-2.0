#pragma once
#include <array>
#include <cassert>
#include <initializer_list>

namespace CU
{
	template<typename T, size_t _Capacity>
	class FixedVector
	{
		static_assert(std::is_trivially_destructible_v<T>, "FixedVector requires trivially destructible types");
	public:
		FixedVector() = default;

		FixedVector(size_t aSize) : m_Size(aSize)
		{
			assert(aSize <= _Capacity);
		}

		FixedVector(std::initializer_list<T> aList)
		{
			assert(aList.size() <= _Capacity);
			for (const auto& v : aList)
			{
				PushBack(v);
			}
		}

		void PushBack(const T& aValue)
		{
			assert(m_Size < _Capacity);
			m_Data[m_Size++] = aValue;
		}

		void PushBack(T&& aValue)
		{
			assert(m_Size < _Capacity);
			m_Data[m_Size++] = std::move(aValue);
		}

		void PopBack()
		{
			assert(m_Size > 0);
			--m_Size;
		}

		template<typename... Args>
		T& EmplaceBack(Args&&... aArgs)
		{
			assert(m_Size < _Capacity);
			m_Data[m_Size] = T(std::forward<Args>(aArgs)...);
			return m_Data[m_Size++];
		}

		size_t Size() const { return m_Size; }
		constexpr size_t Capacity() const { return _Capacity; }

		T* Data() { return m_Data.data(); }
		const T* Data() const { return m_Data.data(); }

		T* Begin() { return m_Data.data(); }
		T* End() { return m_Data.data() + m_Size; }

		const T* Begin() const { return m_Data.data(); }
		const T* End() const { return m_Data.data() + m_Size; }

		T& operator[](size_t i)
		{
			assert(i < m_Size);
			return m_Data[i];
		}

		const T& operator[](size_t i) const
		{
			assert(i < m_Size);
			return m_Data[i];
		}

	private:
		std::array<T, _Capacity> m_Data{};
		size_t m_Size = 0;
	};
}