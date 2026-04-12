#pragma once
#include <xhash>

namespace CU
{
	class UUID
	{
	public:
		constexpr UUID() noexcept : m_UUID(0) {}
		explicit constexpr UUID(uint64_t aValue) noexcept : m_UUID(aValue) {}

		UUID(const UUID&) = default;
		UUID& operator=(const UUID&) = default;

		constexpr bool IsNull() const noexcept { return m_UUID == 0; }

		explicit constexpr operator uint64_t() const noexcept { return m_UUID; }

		friend constexpr bool operator==(UUID a, UUID b) noexcept { return a.m_UUID == b.m_UUID; }
		friend constexpr bool operator!=(UUID a, UUID b) noexcept { return a.m_UUID != b.m_UUID; }

		static UUID Random();

		static const UUID Null;

	private:
		uint64_t m_UUID;
	};
}

namespace std
{
	template<>
	struct hash<CU::UUID>
	{
		size_t operator()(const CU::UUID& uuid) const noexcept
		{
			return std::hash<uint64_t>{}((uint64_t)uuid);
		}
	};
}
