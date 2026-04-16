#pragma once

namespace Epoch::RHI
{
	struct BufferDesc
	{

	};

	class Buffer
	{
	public:
		Buffer() = default;
		virtual ~Buffer() = default;

		const BufferDesc& GetDesc() const { return m_Desc; }

	private:
		BufferDesc m_Desc;
	};
}