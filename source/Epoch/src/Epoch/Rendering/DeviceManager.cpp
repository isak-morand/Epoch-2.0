#include "epch.h"
#include "DeviceManager.h"
#include "Backend/VK/VKDeviceManager.h"
#include "Backend/DX12/DX12DeviceManager.h"

namespace Epoch::RHI
{
#define LOG_TAG LogTags::Renderer

	DeviceManager::DeviceManager() = default;
	DeviceManager::~DeviceManager() = default;

	std::unique_ptr<DeviceManager> DeviceManager::Create(RHI::API aGraphicsAPI)
	{
		switch (aGraphicsAPI)
		{
		case RHI::API::Vulkan:		return std::make_unique<VKDeviceManager>();
		case RHI::API::D3D12:		return std::make_unique<DX12DeviceManager>();
		//case RHI::API::Metal:		return std::make_unique<MTLDevice>(aDesc);
		//case RHI::API::GNM:		return std::make_unique<GNMDevice>(aDesc);
		//case RHI::API::NVN:		return std::make_unique<NVNDevice>(aDesc);
		}

		return nullptr;
	}

	bool DeviceManager::CreateDeviceAndSwapChain(const RendererDesc& aRenderDesc, Window* aWindow)
	{
		m_RenderDesc = aRenderDesc;

		if (!CreateInstance())
		{
			return false;
		}
		LOG_INFO(LOG_TAG, "Instance created");

		if (!CreateDevice(aWindow))
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
