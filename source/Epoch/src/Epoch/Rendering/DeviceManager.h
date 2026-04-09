#pragma once
#include <memory>
#include "RendererDesc.h"

#include <nvrhi/nvrhi.h>

namespace Epoch
{
	class Window;

	class DeviceManager
	{
	public:
		virtual ~DeviceManager();

		static std::unique_ptr<DeviceManager> Create(GraphicsAPI aGraphicsAPI);

		bool CreateDeviceAndSwapChain(Window* aWindow);
		virtual void DestroyDeviceAndSwapChain() = 0;

		virtual void OnWindowResize(uint32_t aWidth, uint32_t aHeight) = 0;

		virtual bool BeginFrame() = 0;
		virtual bool EndFrame() = 0;

		virtual void Render() {}

		virtual nvrhi::DeviceHandle GetDevice() { return nullptr; }

	protected:
		DeviceManager();

		virtual bool CreateInstance() = 0;
		virtual bool CreateDevice() = 0;
		virtual bool CreateSwapChain(Window* aWindow) = 0;

		virtual uint32_t GetCurrentBackBufferIndex() = 0;
		virtual uint32_t GetBackBufferCount() = 0;
	};
}
