#pragma once
#include "Epoch/Rendering/DeviceManager.h"
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_5.h>
#include <dxgidebug.h>

#include <nvrhi/nvrhi.h>

namespace Epoch
{
	class DX12DeviceManager : public DeviceManager
	{
	public:
		DX12DeviceManager();
		~DX12DeviceManager();

		void DestroyDeviceAndSwapChain() override;

		void ResizeSwapChain(uint32_t aWidth, uint32_t aHeight) override;

		bool BeginFrame() override;
		bool EndFrame() override;

		void Render() override;

	protected:
		bool CreateInstance() override;
		bool CreateDevice() override;
		bool CreateSwapChain(Window* aWindow) override;

		uint32_t GetCurrentBackBufferIndex() override;
		uint32_t GetBackBufferCount() override;

	private:
		bool CreateRenderTargets();
		void DestroyRenderTargets();

		void CreateFramebuffers();
		void DestroyFramebuffers();

	private:
		Microsoft::WRL::ComPtr<IDXGIFactory2>			m_DxgiFactory2;
		Microsoft::WRL::ComPtr<ID3D12Device>			m_Device12;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>		m_GraphicsQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>		m_ComputeQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>		m_CopyQueue;
		Microsoft::WRL::ComPtr<IDXGISwapChain3>			m_SwapChain;
		DXGI_SWAP_CHAIN_DESC1							m_SwapChainDesc{};
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC					m_FullScreenDesc{};
		Microsoft::WRL::ComPtr<IDXGIAdapter>			m_DxgiAdapter;
		HWND											m_HWND = nullptr;
		bool											m_TearingSupported = false;

		struct SwapChainBuffer
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> nativeBuffer;
			nvrhi::TextureHandle buffer;
		};
		std::vector<SwapChainBuffer>					m_SwapChainBuffers;
		Microsoft::WRL::ComPtr<ID3D12Fence>				m_FrameFence;
		std::vector<HANDLE>                             m_FrameFenceEvents;

		std::vector<nvrhi::FramebufferHandle>			m_SwapChainFramebuffers;

		UINT64											m_FrameCount = 1;

		nvrhi::DeviceHandle                             m_NvrhiDevice;
		nvrhi::CommandListHandle						m_CommandList;
	};
}
