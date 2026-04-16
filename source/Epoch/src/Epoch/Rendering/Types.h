#pragma once

namespace Epoch::RHI
{
	enum class API
	{
		None,
		Vulkan,
		D3D12,
		//Metal,
		//GNM,
		//NVN
	};

	inline std::string_view APIToString(API aAPI)
	{
		switch (aAPI)
		{
		case API::Vulkan:	return "vulkan";
		case API::D3D12:	return "d3d12";
		default:			return "UNKNOWN";
		}
	}

	inline API APIFromString(std::string_view aAPI)
	{
		if (aAPI == "vulkan")	return API::Vulkan;
		if (aAPI == "d3d12")	return API::D3D12;
		return API::None;
	}

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
