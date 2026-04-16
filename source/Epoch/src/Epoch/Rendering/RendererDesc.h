#pragma once
#include <string_view>
#include "Types.h"

namespace Epoch
{
	struct RendererDesc
	{
		RHI::API graphicsAPI = RHI::API::None;

		bool enableDebugRuntime = false;
		bool enableGPUValidation = false;
		bool enableWarningsAsErrors = false;

		//TextureFormat swapChainFormat = TextureFormat::RGBA8_UNORM;
		uint32_t swapChainBufferCount = 3;

		uint32_t framesInFlight = 2;
	};
}
