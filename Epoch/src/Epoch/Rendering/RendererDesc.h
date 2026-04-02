#pragma once

namespace Epoch
{
    enum class GraphicsAPI
    {
        None,
        Vulkan,
        D3D12,
        Metal,
        GNM,
        NVN
    };

    struct DeviceDesc
    {

    };

    struct RendererDesc
    {
        GraphicsAPI API = GraphicsAPI::None;
    };
}
