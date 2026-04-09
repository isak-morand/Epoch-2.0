#include "epch.h"
#include "DeviceManager.h"
#include "Backend/VK/VKDeviceManager.h"
#include "Backend/DX12/DX12DeviceManager.h"

namespace Epoch
{
#define LOG_TAG LogTags::Renderer

	DeviceManager::DeviceManager() = default;
	DeviceManager::~DeviceManager() = default;

	std::unique_ptr<DeviceManager> DeviceManager::Create(GraphicsAPI aGraphicsAPI)
	{
		switch (aGraphicsAPI)
		{
		case GraphicsAPI::Vulkan:			return std::make_unique<VKDeviceManager>();
		case GraphicsAPI::D3D12:			return std::make_unique<DX12DeviceManager>();
		//case Epoch::GraphicsAPI::Metal:	return std::make_unique<MTLDevice>(aDesc);
		//case Epoch::GraphicsAPI::GNM:		return std::make_unique<GNMDevice>(aDesc);
		//case Epoch::GraphicsAPI::NVN:		return std::make_unique<NVNDevice>(aDesc);
		}

		return nullptr;
	}

	bool DeviceManager::CreateDeviceAndSwapChain(Window* aWindow)
	{
		if (!CreateInstance())
		{
			return false;
		}
		LOG_INFO(LOG_TAG, "Instance created");

		if (!CreateDevice())
		{
			return false;
		}
		LOG_INFO(LOG_TAG, "Device created");

		if (!CreateSwapChain(aWindow))
		{
			return false;
		}
		LOG_INFO(LOG_TAG, "Swap Chain created");

		return true;
	}
}
