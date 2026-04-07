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
		Microsoft::WRL::ComPtr<IDXGIFactory2>			myDxgiFactory2;
		Microsoft::WRL::ComPtr<ID3D12Device>			myDevice12;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>		myGraphicsQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>		myComputeQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue>		myCopyQueue;
		Microsoft::WRL::ComPtr<IDXGISwapChain3>			mySwapChain;
		DXGI_SWAP_CHAIN_DESC1							mySwapChainDesc{};
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC					myFullScreenDesc{};
		Microsoft::WRL::ComPtr<IDXGIAdapter>			myDxgiAdapter;
		HWND											myHWND = nullptr;
		bool											myTearingSupported = false;

		struct SwapChainBuffer
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> nativeBuffer;
			nvrhi::TextureHandle buffer;
		};
		std::vector<SwapChainBuffer>					mySwapChainBuffers;
		Microsoft::WRL::ComPtr<ID3D12Fence>				myFrameFence;
		std::vector<HANDLE>                             myFrameFenceEvents;

		std::vector<nvrhi::FramebufferHandle>			mySwapChainFramebuffers;

		UINT64											myFrameCount = 1;

		nvrhi::DeviceHandle                             myNvrhiDevice;
		nvrhi::CommandListHandle						myCommandList;
	};
}
