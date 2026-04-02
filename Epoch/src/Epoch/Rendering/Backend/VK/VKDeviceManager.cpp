#include "epch.h"
#include "VKDeviceManager.h"

namespace Epoch
{
	VKDeviceManager::VKDeviceManager() = default;
	VKDeviceManager::~VKDeviceManager() = default;

	void VKDeviceManager::DestroyDeviceAndSwapChain()
	{
	}

	void VKDeviceManager::ResizeSwapChain(uint32_t aWidth, uint32_t aHeight)
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
