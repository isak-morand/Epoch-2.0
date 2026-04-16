#pragma once

namespace Epoch::RHI
{
	struct TextureDesc
	{

	};

	class Texture
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;

		const TextureDesc& GetDesc() const { return m_Desc; }

	private:
		TextureDesc m_Desc;
	};
}