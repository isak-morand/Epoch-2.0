#pragma once

namespace Epoch
{
	enum class TextureFormat
	{
		None = 0,

		RGBA8_UNORM,
		RGBA8_SRGB,
		RGBA32_FLOAT,
		R11G11B10_FLOAT,

		RG8_UNORM,
		RG16_FLOAT,
		RG16_UNORM,

		DEPTH32,

		BC3,
		BC5,
		BC7,
		BC3_SRGB,
		BC7_SRGB,
	};
}
