#include "epch.h"
#include "DX12DeviceManager.h"
#include "Epoch/Core/Window.h"
#include <nvrhi/d3d12.h>
#include <nvrhi/validation.h>
#include <nvrhi/utils.h>

namespace Epoch
{
#define LOG_TAG LogTags::Renderer

	DX12DeviceManager::DX12DeviceManager() = default;
	DX12DeviceManager::~DX12DeviceManager() = default;

	void DX12DeviceManager::DestroyDeviceAndSwapChain()
	{
		DestroyRenderTargets();

		myNvrhiDevice = nullptr;

		for (auto fenceEvent : myFrameFenceEvents)
		{
			WaitForSingleObject(fenceEvent, INFINITE);
			CloseHandle(fenceEvent);
		}

		myFrameFenceEvents.clear();

		if (mySwapChain)
		{
			mySwapChain->SetFullscreenState(false, nullptr);
		}

		mySwapChainBuffers.clear();

		myFrameFence = nullptr;
		mySwapChain = nullptr;
		myGraphicsQueue = nullptr;
		myComputeQueue = nullptr;
		myCopyQueue = nullptr;
		myDevice12 = nullptr;
	}

	void DX12DeviceManager::ResizeSwapChain(uint32_t aWidth, uint32_t aHeight)
	{
		DestroyRenderTargets();

		if (!myNvrhiDevice) return;
		if (!mySwapChain) return;

		const HRESULT hr = mySwapChain->ResizeBuffers(
			mySwapChainDesc.BufferCount,
			aWidth,
			aHeight,
			DXGI_FORMAT_UNKNOWN,
			mySwapChainDesc.Flags);
		
		if (FAILED(hr))
		{
			LOG_FATAL(LOG_TAG, "ResizeBuffers failed!");
		}

		bool ret = CreateRenderTargets();
		if (!ret)
		{
			LOG_FATAL(LOG_TAG, "CreateRenderTarget failed");
		}
	}

	bool DX12DeviceManager::BeginFrame()
	{
		UINT bufferIndex = GetCurrentBackBufferIndex();
		WaitForSingleObject(myFrameFenceEvents[bufferIndex], INFINITE);

		return true;
	}

	bool DX12DeviceManager::EndFrame()
	{
		const uint32_t bufferIndex = GetCurrentBackBufferIndex();

		UINT presentFlags = 0;
		if (/*!m_DeviceParams.vsyncEnabled && */myFullScreenDesc.Windowed && myTearingSupported)
		{
			presentFlags |= DXGI_PRESENT_ALLOW_TEARING;
		}

		HRESULT result = mySwapChain->Present(0/*myDeviceParams.vsyncEnabled ? 1 : 0*/, presentFlags);

		myFrameFence->SetEventOnCompletion(myFrameCount, myFrameFenceEvents[bufferIndex]);
		myGraphicsQueue->Signal(myFrameFence.Get(), myFrameCount);
		myFrameCount++;

		myNvrhiDevice->runGarbageCollection();

		return SUCCEEDED(result);
	}

	void DX12DeviceManager::Render()
	{
		if (!myCommandList)
		{
			myCommandList = myNvrhiDevice->createCommandList();
		}

		myCommandList->open();
		myCommandList->clearTextureFloat(mySwapChainBuffers[GetCurrentBackBufferIndex()].buffer, nvrhi::AllSubresources, nvrhi::Color(1, 0, 0, 1));
		myCommandList->close();

		myNvrhiDevice->executeCommandList(myCommandList);
	}

	bool DX12DeviceManager::CreateInstance()
	{
		if (!myDxgiFactory2)
		{
			HRESULT hres = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG/*myEnableDebugRuntime ? DXGI_CREATE_FACTORY_DEBUG : 0*/, IID_PPV_ARGS(&myDxgiFactory2));
			if (hres != S_OK)
			{
				LOG_ERROR(LOG_TAG, "Failed to create DXGIFactory2!");
				return false;
			}
		}

		return true;
	}

	bool DX12DeviceManager::CreateDevice()
	{
		//if (myEnableDebugRuntime)
		{
			Microsoft::WRL::ComPtr<ID3D12Debug> pDebug;
			HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(pDebug.GetAddressOf()));

			if (SUCCEEDED(hr))
			{
				pDebug->EnableDebugLayer();
			}
			else
			{
				LOG_WARNING(LOG_TAG, "Cannot enable DX12 debug runtime, ID3D12Debug is not available.");
			}
		}

		//if (m_DeviceParams.enableGPUValidation)
		//{
		//	Microsoft::WRL::ComPtr<ID3D12Debug3> debugController3;
		//	HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(debugController3.GetAddressOf()));
		//
		//	if (SUCCEEDED(hr))
		//	{
		//		debugController3->SetEnableGPUBasedValidation(true);
		//	}
		//	else
		//	{
		//		LOG_WARNING(LOG_TAG, "Cannot enable GPU-based validation, ID3D12Debug3 is not available.");
		//	}
		//}

		int adapterIndex = 0;

		if (FAILED(myDxgiFactory2->EnumAdapters(adapterIndex, myDxgiAdapter.GetAddressOf())))
		{
			if (adapterIndex == 0)
			{
				LOG_ERROR(LOG_TAG, "Cannot find any DXGI adapters in the system.");
			}
			else
			{
				LOG_ERROR(LOG_TAG, "The specified DXGI adapter {} does not exist.", adapterIndex);
			}

			return false;
		}

		//{
		//	DXGI_ADAPTER_DESC aDesc;
		//	myDxgiAdapter->GetDesc(&aDesc);
		//
		//	m_RendererString = GetAdapterName(aDesc);
		//	m_IsNvidia = IsNvDeviceID(aDesc.VendorId);
		//}

		HRESULT hr = D3D12CreateDevice(myDxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(myDevice12.GetAddressOf()));
		
		if (FAILED(hr))
		{
			LOG_ERROR(LOG_TAG, "D3D12CreateDevice failed! Error code: 0x%08x", hr);
			return false;
		}

		//if (myEnableDebugRuntime)
		{
			Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfoQueue;
			myDevice12->QueryInterface(pInfoQueue.GetAddressOf());

			if (pInfoQueue)
			{
#ifdef _DEBUG
				//if (m_DeviceParams.enableWarningsAsErrors)
				{
					pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
				}
				pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
				pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif

				D3D12_MESSAGE_ID disableMessageIDs[] =
				{
					D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
					D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
					D3D12_MESSAGE_ID_COMMAND_LIST_STATIC_DESCRIPTOR_RESOURCE_DIMENSION_MISMATCH,	// descriptor validation doesn't understand acceleration structures
					D3D12_MESSAGE_ID_CREATERESOURCE_STATE_IGNORED,									// NGX currently generates benign resource creation warnings
				};

				D3D12_INFO_QUEUE_FILTER filter = {};
				filter.DenyList.pIDList = disableMessageIDs;
				filter.DenyList.NumIDs = sizeof(disableMessageIDs) / sizeof(disableMessageIDs[0]);
				pInfoQueue->AddStorageFilterEntries(&filter);
			}
		}

		D3D12_COMMAND_QUEUE_DESC queueDesc;
		ZeroMemory(&queueDesc, sizeof(queueDesc));
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.NodeMask = 1;
		hr = myDevice12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(myGraphicsQueue.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}
		myGraphicsQueue->SetName(L"Graphics Queue");

		//if (m_DeviceParams.enableComputeQueue)
		//{
		//	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		//	hr = myDevice12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(myComputeQueue.GetAddressOf()));
		//	if (FAILED(hr))
		//	{
		//		return false;
		//	}
		//	myComputeQueue->SetName(L"Compute Queue");
		//}
		//
		//if (m_DeviceParams.enableCopyQueue)
		//{
		//	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		//	hr = myDevice12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(myCopyQueue.GetAddressOf()));
		//	if (FAILED(hr))
		//	{
		//		return false;
		//	}
		//	myCopyQueue->SetName(L"Copy Queue");
		//}

		nvrhi::d3d12::DeviceDesc deviceDesc;
		//deviceDesc.errorCB = myDeviceParams.messageCallback ? m_DeviceParams.messageCallback : &DefaultMessageCallback::GetInstance();
		deviceDesc.pDevice = myDevice12.Get();
		deviceDesc.pGraphicsCommandQueue = myGraphicsQueue.Get();
		deviceDesc.pComputeCommandQueue = myComputeQueue.Get();
		deviceDesc.pCopyCommandQueue = myCopyQueue.Get();
		//deviceDesc.logBufferLifetime = myDeviceParams.logBufferLifetime;
		deviceDesc.enableHeapDirectlyIndexed = false;

		myNvrhiDevice = nvrhi::d3d12::createDevice(deviceDesc);

		//if (m_DeviceParams.enableNvrhiValidationLayer)
		{
			myNvrhiDevice = nvrhi::validation::createValidationLayer(myNvrhiDevice);
		}

		return true;
	}

	bool DX12DeviceManager::CreateSwapChain(Window* aWindow)
	{
		HWND hWnd = (HWND)aWindow->GetNativeWindow();

		HRESULT hr = E_FAIL;

		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		UINT width = clientRect.right - clientRect.left;
		UINT height = clientRect.bottom - clientRect.top;

		ZeroMemory(&mySwapChainDesc, sizeof(mySwapChainDesc));
		mySwapChainDesc.Width = width;
		mySwapChainDesc.Height = height;
		mySwapChainDesc.SampleDesc.Count = 1;//m_DeviceParams.swapChainSampleCount;
		mySwapChainDesc.SampleDesc.Quality = 0;
		mySwapChainDesc.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;//m_DeviceParams.swapChainUsage;
		mySwapChainDesc.BufferCount = 3;//m_DeviceParams.swapChainBufferCount;
		mySwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		mySwapChainDesc.Flags = 0;//m_DeviceParams.allowModeSwitch ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;

		mySwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//switch (m_DeviceParams.swapChainFormat)
		//{
		//case nvrhi::Format::SRGBA8_UNORM:
		//	m_SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//	break;
		//case nvrhi::Format::SBGRA8_UNORM:
		//	m_SwapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		//	break;
		//default:
		//	m_SwapChainDesc.Format = nvrhi::d3d12::convertFormat(m_DeviceParams.swapChainFormat);
		//	break;
		//}

		Microsoft::WRL::ComPtr<IDXGIFactory5> pDxgiFactory5;
		if (SUCCEEDED(myDxgiFactory2->QueryInterface(IID_PPV_ARGS(pDxgiFactory5.GetAddressOf()))))
		{
			BOOL supported = 0;
			if (SUCCEEDED(pDxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &supported, sizeof(supported))))
			{
				myTearingSupported = (supported != 0);
			}
		}

		if (myTearingSupported)
		{
			mySwapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		}

		myFullScreenDesc = {};
		myFullScreenDesc.RefreshRate.Numerator = 0;//myDeviceParams.refreshRate;
		myFullScreenDesc.RefreshRate.Denominator = 1;
		myFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
		myFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		myFullScreenDesc.Windowed = true;//!m_DeviceParams.startFullscreen;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapChain1;
		hr = myDxgiFactory2->CreateSwapChainForHwnd(myGraphicsQueue.Get(), hWnd, &mySwapChainDesc, &myFullScreenDesc, nullptr, &pSwapChain1);
		if (FAILED(hr))
		{
			return false;
		}

		hr = pSwapChain1->QueryInterface(IID_PPV_ARGS(mySwapChain.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		if (!CreateRenderTargets())
		{
			return false;
		}

		hr = myDevice12->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(myFrameFence.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		for (UINT bufferIndex = 0; bufferIndex < mySwapChainDesc.BufferCount; bufferIndex++)
		{
			myFrameFenceEvents.push_back(CreateEvent(nullptr, false, true, nullptr));
		}

		return true;
	}

	uint32_t DX12DeviceManager::GetCurrentBackBufferIndex()
	{
		return mySwapChain->GetCurrentBackBufferIndex();
	}

	uint32_t DX12DeviceManager::GetBackBufferCount()
	{
		return mySwapChainDesc.BufferCount;
	}

	bool DX12DeviceManager::CreateRenderTargets()
	{
		mySwapChainBuffers.resize(mySwapChainDesc.BufferCount);

		for (UINT n = 0; n < mySwapChainDesc.BufferCount; n++)
		{
			const HRESULT hr = mySwapChain->GetBuffer(n, IID_PPV_ARGS(mySwapChainBuffers[n].nativeBuffer.GetAddressOf()));
			if (FAILED(hr))
			{
				return false;
			}

			nvrhi::TextureDesc textureDesc;
			textureDesc.width = mySwapChainDesc.Width;
			textureDesc.height = mySwapChainDesc.Height;
			textureDesc.sampleCount = mySwapChainDesc.SampleDesc.Count;
			textureDesc.sampleQuality = mySwapChainDesc.SampleDesc.Quality;
			textureDesc.format = nvrhi::Format::RGBA8_UNORM;
			textureDesc.debugName = "SwapChainBuffer";
			textureDesc.isRenderTarget = true;
			textureDesc.isUAV = false;
			textureDesc.initialState = nvrhi::ResourceStates::Present;
			textureDesc.keepInitialState = true;

			mySwapChainBuffers[n].Buffer = myNvrhiDevice->createHandleForNativeTexture(nvrhi::ObjectTypes::D3D12_Resource, nvrhi::Object(mySwapChainBuffers[n].NativeBuffer.Get()), textureDesc);
		}

		CreateFramebuffers();

		return true;
	}
	
	void DX12DeviceManager::DestroyRenderTargets()
	{
		DestroyFramebuffers();

		if (myNvrhiDevice)
		{
			myNvrhiDevice->waitForIdle();
			myNvrhiDevice->runGarbageCollection();
		}

		for (auto e : myFrameFenceEvents)
		{
			SetEvent(e);
		}

		mySwapChainBuffers.clear();
	}
	
	void DX12DeviceManager::CreateFramebuffers()
	{
		mySwapChainFramebuffers.resize(mySwapChainDesc.BufferCount);
		for (size_t i = 0; i < mySwapChainDesc.BufferCount; i++)
		{
			nvrhi::FramebufferDesc framebufferDesc = nvrhi::FramebufferDesc().addColorAttachment(mySwapChainBuffers[i].buffer);
			mySwapChainFramebuffers[i] = myNvrhiDevice->createFramebuffer(framebufferDesc);
		}
	}

	void DX12DeviceManager::DestroyFramebuffers()
	{
		mySwapChainFramebuffers.clear();
	}
}
