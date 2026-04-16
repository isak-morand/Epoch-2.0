#pragma once
#include "Epoch/Rendering/DeviceManager.h"

namespace Epoch::RHI
{
	class VKDeviceManager : public DeviceManager
	{
	public:
		VKDeviceManager();
		~VKDeviceManager() override;

		void DestroyDeviceAndSwapChain() override;

		void OnWindowResize(uint32_t aWidth, uint32_t aHeight) override;

		bool BeginFrame() override;
		bool EndFrame() override;

		std::shared_ptr<RHI::Buffer> CreateBuffer(const RHI::BufferDesc& aDesc) override;
		std::shared_ptr<RHI::Texture> CreateTexture(const RHI::TextureDesc& aDesc) override;

	protected:
		bool CreateInstance() override;
		bool CreateDevice() override;
		bool CreateSwapChain(Window* aWindow) override;

		uint32_t GetCurrentBackBufferIndex() override;
		uint32_t GetBackBufferCount() override;

	private:
		void ResizeSwapChain(uint32_t aWidth, uint32_t aHeight);

	private:

	};
}
