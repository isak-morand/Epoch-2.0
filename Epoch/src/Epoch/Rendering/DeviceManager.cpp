#include "epch.h"
#include "DeviceManager.h"
#include "Backend/VK/VKDeviceManager.h"
#include "Backend/DX12/DX12DeviceManager.h"

namespace Epoch
{
    std::unique_ptr<DeviceManager> DeviceManager::Create(GraphicsAPI aGraphicsAPI)
    {
        switch (aGraphicsAPI)
        {
        case GraphicsAPI::Vulkan:    return std::make_unique<VKDeviceManager>();
        case GraphicsAPI::D3D12:     return std::make_unique<DX12DeviceManager>();
        //case Epoch::GraphicsAPI::Metal:     return std::make_unique<MTLDevice>(aDesc);
        //case Epoch::GraphicsAPI::GNM:       return std::make_unique<GNMDevice>(aDesc);
        //case Epoch::GraphicsAPI::NVN:       return std::make_unique<NVNDevice>(aDesc);
        }

        return nullptr;
    }

    bool DeviceManager::CreateDeviceAndSwapChain(Window* aWindow)
    {
        if (!CreateInstance())
        {
            return false;
        }
        
        if (!CreateDevice())
        {
            return false;
        }

        if (!CreateSwapChain(aWindow))
        {
            return false;
        }

        return true;
    }
}
