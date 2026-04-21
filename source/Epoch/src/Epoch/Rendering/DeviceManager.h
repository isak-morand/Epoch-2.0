#pragma once
#include <memory>
#include "RendererDesc.h"
#include "Resources/Buffer.h"
#include "Resources/Texture.h"

#include <nvrhi/nvrhi.h>

namespace Epoch
{
	class Window;
}

namespace Epoch::RHI
{
	class DeviceManager
	{
	public:
		virtual ~DeviceManager();

		static std::unique_ptr<DeviceManager> Create(RHI::API aGraphicsAPI);

		bool CreateDeviceAndSwapChain(const RendererDesc& aRenderDesc, Window* aWindow);
		virtual void DestroyDeviceAndSwapChain() = 0;

		virtual void OnWindowResize(uint32_t aWidth, uint32_t aHeight) = 0;

		virtual bool BeginFrame() = 0;
		virtual bool EndFrame() = 0;

		virtual nvrhi::IDevice* GetDevice() = 0;
		virtual nvrhi::IFramebuffer* GetCurrentFramebuffer() = 0;
		virtual nvrhi::ITexture* GetCurrentFramebufferImage() = 0;

		virtual std::shared_ptr<RHI::Buffer> CreateBuffer(const RHI::BufferDesc& aDesc) = 0;
		virtual std::shared_ptr<RHI::Texture> CreateTexture(const RHI::TextureDesc& aDesc) = 0;

		RHI::API GetAPI() const { return m_RenderDesc.graphicsAPI; }

	protected:
		DeviceManager();

		virtual bool CreateInstance() = 0;
		virtual bool CreateDevice(Window* aWindow) = 0;
		virtual bool CreateSwapChain(Window* aWindow) = 0;

		virtual uint32_t GetCurrentBackBufferIndex() = 0;
		virtual uint32_t GetBackBufferCount() = 0;

	protected:
		RendererDesc m_RenderDesc;

		std::string m_AdaptorName;
	};
}
