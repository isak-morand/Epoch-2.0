#pragma once
#include "Format.h"

namespace Epoch
{
	enum class GraphicsAPI
	{
		None,
		Vulkan,
		D3D12,
		//Metal,
		//GNM,
		//NVN
	};

	inline std::string_view GraphicsAPIToString(GraphicsAPI aAPI)
	{
		switch (aAPI)
		{
		case GraphicsAPI::Vulkan:	return "vulkan";
		case GraphicsAPI::D3D12:	return "d3d12";
		default:					return "UNKNOWN";
		}
	}

	inline GraphicsAPI GraphicsAPIFromString(std::string_view aAPI)
	{
		if (aAPI == "vulkan") return GraphicsAPI::Vulkan;
		if (aAPI == "d3d12") return GraphicsAPI::D3D12;
		return GraphicsAPI::None;
	}

	struct RendererDesc
	{
		GraphicsAPI graphicsAPI = GraphicsAPI::None;

		bool enableDebugRuntime = false;
		bool enableGPUValidation = false;
		bool enableWarningsAsErrors = false;

		//TextureFormat swapChainFormat = TextureFormat::RGBA8_UNORM;
		uint32_t swapChainBufferCount = 3;

		uint32_t framesInFlight = 2;
	};
}
