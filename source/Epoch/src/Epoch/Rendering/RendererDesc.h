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

	struct RendererDesc
	{
		GraphicsAPI graphicsAPI = GraphicsAPI::None;

		bool enableDebugRuntime = false;
		bool enableWarningsAsErrors = false;
		bool enableGPUValidation = false; // Affects only DX12

		//TextureFormat swapChainFormat = TextureFormat::RGBA8_UNORM;
		uint32_t swapChainBufferCount = 3;

		uint32_t framesInFlight = 2;
	};
}
