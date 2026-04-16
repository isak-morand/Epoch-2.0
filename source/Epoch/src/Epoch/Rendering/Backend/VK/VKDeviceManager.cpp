#include "epch.h"
#include "VKDeviceManager.h"

namespace Epoch::RHI
{
	VKDeviceManager::VKDeviceManager() = default;
	VKDeviceManager::~VKDeviceManager() = default;

	void VKDeviceManager::DestroyDeviceAndSwapChain()
	{
	}

	void VKDeviceManager::OnWindowResize(uint32_t aWidth, uint32_t aHeight)
	{
	}

	bool VKDeviceManager::BeginFrame()
	{
		return false;
	}

	bool VKDeviceManager::EndFrame()
	{
		return false;
	}

	std::shared_ptr<RHI::Buffer> VKDeviceManager::CreateBuffer(const RHI::BufferDesc& aDesc)
	{
		return std::shared_ptr<RHI::Buffer>();
	}

	std::shared_ptr<RHI::Texture> VKDeviceManager::CreateTexture(const RHI::TextureDesc& aDesc)
	{
		return std::shared_ptr<RHI::Texture>();
	}

	bool VKDeviceManager::CreateInstance()
	{
		return false;
	}

	bool VKDeviceManager::CreateDevice()
	{
		return false;
	}

	bool VKDeviceManager::CreateSwapChain(Window* aWindow)
	{
		return false;
	}

	uint32_t VKDeviceManager::GetCurrentBackBufferIndex()
	{
		return 0;
	}
	
	uint32_t VKDeviceManager::GetBackBufferCount()
	{
		return 0;
	}
}
