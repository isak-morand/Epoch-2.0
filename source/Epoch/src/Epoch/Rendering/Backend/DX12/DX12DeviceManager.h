#pragma once
#include "Epoch/Rendering/DeviceManager.h"
#include <d3d12.h>
#include <dxgi1_5.h>
#include <dxgidebug.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <nvrhi/nvrhi.h>

namespace Epoch::RHI
{
	class DX12DeviceManager : public DeviceManager
	{
	public:
		DX12DeviceManager();
		~DX12DeviceManager() override;

		void DestroyDeviceAndSwapChain() override;

		void OnWindowResize(uint32_t aWidth, uint32_t aHeight) override;

		bool BeginFrame() override;
		bool EndFrame() override;

		nvrhi::IDevice* GetDevice() override { return m_NvrhiDevice; }
		nvrhi::IFramebuffer* GetCurrentFramebuffer() override { return m_SwapChainFramebuffers[GetCurrentBackBufferIndex()]; }
		nvrhi::ITexture* GetCurrentFramebufferImage() override { return m_RHISwapChainBuffers[GetCurrentBackBufferIndex()]; }

		std::shared_ptr<RHI::Buffer> CreateBuffer(const RHI::BufferDesc& aDesc) override;
		std::shared_ptr<RHI::Texture> CreateTexture(const RHI::TextureDesc& aDesc) override;

	protected:
		bool CreateInstance() override;
		bool CreateDevice(Window* aWindow) override;
		bool CreateSwapChain(Window* aWindow) override;

		uint32_t GetCurrentBackBufferIndex() override;
		uint32_t GetBackBufferCount() override;

	private:
		std::string GetAdapterName(DXGI_ADAPTER_DESC const& aDesc);

		bool CreateRenderTargets();
		void DestroyRenderTargets();
		void ResizeSwapChain();

		void CreateFramebuffers();
		void DestroyFramebuffers();

	private:
		ComPtr<IDXGIFactory2> m_DxgiFactory2;

		ComPtr<IDXGIAdapter> m_DxgiAdapter;

		ComPtr<ID3D12Device> m_Device12;
		ComPtr<ID3D12CommandQueue> m_GraphicsQueue;
		ComPtr<ID3D12CommandQueue> m_ComputeQueue;
		ComPtr<ID3D12CommandQueue> m_CopyQueue;

		ComPtr<IDXGISwapChain3> m_SwapChain;
		DXGI_SWAP_CHAIN_DESC1 m_SwapChainDesc{};
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC m_FullScreenDesc{};

		HWND m_HWND = nullptr;
		bool m_TearingSupported = false;

		std::vector<ComPtr<ID3D12Resource>> m_SwapChainBuffers;
		std::vector<nvrhi::TextureHandle> m_RHISwapChainBuffers;
		ComPtr<ID3D12Fence> m_FrameFence;
		std::vector<HANDLE> m_FrameFenceEvents;

		std::vector<nvrhi::FramebufferHandle> m_SwapChainFramebuffers;

		bool m_ShouldResize = false;

		UINT64 m_FrameCount = 0;

		nvrhi::DeviceHandle m_NvrhiDevice;
	};
}
