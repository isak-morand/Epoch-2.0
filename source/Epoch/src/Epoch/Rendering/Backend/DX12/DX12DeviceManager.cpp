#include "epch.h"
#include "DX12DeviceManager.h"
#include "Epoch/Core/Window.h"
#include "Epoch/Rendering/ShaderCompiler.h"
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

		m_Pipeline = nullptr;

		m_NvrhiDevice = nullptr;

		for (auto fenceEvent : m_FrameFenceEvents)
		{
			WaitForSingleObject(fenceEvent, INFINITE);
			CloseHandle(fenceEvent);
		}

		m_FrameFenceEvents.clear();

		if (m_SwapChain)
		{
			m_SwapChain->SetFullscreenState(false, nullptr);
		}

		m_SwapChainBuffers.clear();

		m_FrameFence = nullptr;
		m_SwapChain = nullptr;
		m_GraphicsQueue = nullptr;
		m_ComputeQueue = nullptr;
		m_CopyQueue = nullptr;
		m_Device12 = nullptr;
	}

	void DX12DeviceManager::OnWindowResize(uint32_t aWidth, uint32_t aHeight)
	{
		if (aWidth == 0 || aHeight == 0 ||
			(aWidth == m_SwapChainDesc.Width && aHeight == m_SwapChainDesc.Height))
		{
			return;
		}

		m_SwapChainDesc.Width = aWidth;
		m_SwapChainDesc.Height = aHeight;

		m_ShouldResize = true;
	}

	bool DX12DeviceManager::BeginFrame()
	{
		UINT bufferIndex = GetCurrentBackBufferIndex();
		WaitForSingleObject(m_FrameFenceEvents[bufferIndex], INFINITE);

		if (m_ShouldResize)
		{
			ResizeSwapChain();
			m_ShouldResize = false;
		}

		return true;
	}

	bool DX12DeviceManager::EndFrame()
	{
		const uint32_t bufferIndex = GetCurrentBackBufferIndex();

		UINT presentFlags = 0;
		if (m_FullScreenDesc.Windowed && m_TearingSupported)
		{
			presentFlags |= DXGI_PRESENT_ALLOW_TEARING;
		}

		HRESULT result = m_SwapChain->Present(0, presentFlags);

		m_FrameFence->SetEventOnCompletion(m_FrameCount, m_FrameFenceEvents[bufferIndex]);
		m_GraphicsQueue->Signal(m_FrameFence.Get(), m_FrameCount);
		m_FrameCount++;

		m_NvrhiDevice->runGarbageCollection();

		return SUCCEEDED(result);
	}

	void DX12DeviceManager::Render()
	{
		if (!m_CommandList)
		{
			m_CommandList = m_NvrhiDevice->createCommandList();

			ShaderCompiler shaderCompiler;

			{
				ShaderCompileInput vsIn;

				vsIn.source = R"(
				struct VSOut
				{
					float4 position : SV_Position;
				};
				
				VSOut main(uint vertexID : SV_VertexID)
				{
					float2 positions[3] =
					{
						float2( 0.0,  0.5),
						float2( 0.5, -0.5),
						float2(-0.5, -0.5)
					};

					VSOut output;
					output.position = float4(positions[vertexID], 0.0, 1.0);
					return output;
				}
				)";

				vsIn.entryPoint = "main";
				vsIn.stage = ShaderStage::Vertex;
				vsIn.optimize = true;
				vsIn.debug = true;

				ShaderCompileOutput vsOut;
				shaderCompiler.Compile(vsIn, vsOut);

				nvrhi::ShaderDesc desc;
				desc.setShaderType(nvrhi::ShaderType::Vertex);

				m_VertexShader = m_NvrhiDevice->createShader(desc, vsOut.bytecode.data(), vsOut.bytecode.size());
			}

			{
				ShaderCompileInput psIn;

				psIn.source = R"(
				float4 main() : SV_Target
				{
					return float4(0.2, 0.8, 0.3, 1.0);
				}
				)";

				psIn.entryPoint = "main";
				psIn.stage = ShaderStage::Pixel;
				psIn.optimize = true;
				psIn.debug = true;

				ShaderCompileOutput psOut;
				shaderCompiler.Compile(psIn, psOut);

				nvrhi::ShaderDesc desc;
				desc.setShaderType(nvrhi::ShaderType::Pixel);

				m_PixelShader = m_NvrhiDevice->createShader(desc, psOut.bytecode.data(), psOut.bytecode.size());
			}

			nvrhi::GraphicsPipelineDesc pipelineDesc;
			pipelineDesc.VS = m_VertexShader;
			pipelineDesc.PS = m_PixelShader;
			pipelineDesc.renderState.depthStencilState.depthTestEnable = false;

			nvrhi::FramebufferInfo fbInfo;
			fbInfo.addColorFormat(nvrhi::Format::RGBA8_UNORM);

			m_Pipeline = m_NvrhiDevice->createGraphicsPipeline(pipelineDesc, fbInfo);
		}

		m_CommandList->open();
		m_CommandList->clearTextureFloat(m_SwapChainBuffers[GetCurrentBackBufferIndex()].buffer, nvrhi::AllSubresources, nvrhi::Color(0, 0, 0, 0));

		nvrhi::GraphicsState state;
		state.pipeline = m_Pipeline;
		state.framebuffer = m_SwapChainFramebuffers[GetCurrentBackBufferIndex()];
		state.viewport.addViewportAndScissorRect(m_SwapChainFramebuffers[GetCurrentBackBufferIndex()]->getFramebufferInfo().getViewport());

		m_CommandList->setGraphicsState(state);

		m_CommandList->draw({ 3 });

		m_CommandList->close();

		m_NvrhiDevice->executeCommandList(m_CommandList);
	}

	bool DX12DeviceManager::CreateInstance()
	{
		if (!m_DxgiFactory2)
		{
			HRESULT hres = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG/*m_EnableDebugRuntime ? DXGI_CREATE_FACTORY_DEBUG : 0*/, IID_PPV_ARGS(&m_DxgiFactory2));
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
		//if (m_EnableDebugRuntime)
		if (false)
		{
			Microsoft::WRL::ComPtr<ID3D12Debug> pDebug;
			HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(pDebug.GetAddressOf()));

			if (SUCCEEDED(hr))
			{
				pDebug->EnableDebugLayer();
				LOG_TRACE(LOG_TAG, "DX12 debug runtime enabled");
			}
			else
			{
				LOG_WARN(LOG_TAG, "Cannot enable DX12 debug runtime, ID3D12Debug is not available.");
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
		//		LOG_TRACE(LOG_TAG, "DX12 GPU validation enabled");
		//	}
		//	else
		//	{
		//		LOG_WARNING(LOG_TAG, "Cannot enable GPU-based validation, ID3D12Debug3 is not available.");
		//	}
		//}

		int adapterIndex = 0;

		if (FAILED(m_DxgiFactory2->EnumAdapters(adapterIndex, m_DxgiAdapter.GetAddressOf())))
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
		//	m_DxgiAdapter->GetDesc(&aDesc);
		//
		//	m_RendererString = GetAdapterName(aDesc);
		//	m_IsNvidia = IsNvDeviceID(aDesc.VendorId);
		//}

		HRESULT hr = D3D12CreateDevice(m_DxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(m_Device12.GetAddressOf()));
		
		if (FAILED(hr))
		{
			LOG_ERROR(LOG_TAG, "D3D12CreateDevice failed! Error code: {}", hr);
			return false;
		}

		//if (m_EnableDebugRuntime)
		if (false)
		{
			Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfoQueue;
			m_Device12->QueryInterface(pInfoQueue.GetAddressOf());

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
		hr = m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_GraphicsQueue.GetAddressOf()));
		if (FAILED(hr))
		{
			LOG_ERROR(LOG_TAG, "CreateCommandQueue failed! Error code: {}", hr);
			return false;
		}
		m_GraphicsQueue->SetName(L"Graphics Queue");

		LOG_TRACE(LOG_TAG, "DX12 graphics queue created");

		//if (m_DeviceParams.enableComputeQueue)
		//{
		//	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		//	hr = m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_ComputeQueue.GetAddressOf()));
		//	if (FAILED(hr))
		//	{
		//		LOG_ERROR(LOG_TAG, "CreateCommandQueue failed! Error code: {}", hr);
		//		return false;
		//	}
		//	m_ComputeQueue->SetName(L"Compute Queue");
		//	LOG_TRACE(LOG_TAG, "DX12 compute queue created");
		//}
		//
		//if (m_DeviceParams.enableCopyQueue)
		//{
		//	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		//	hr = m_Device12->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_CopyQueue.GetAddressOf()));
		//	if (FAILED(hr))
		//	{
		//		LOG_ERROR(LOG_TAG, "CreateCommandQueue failed! Error code: {}", hr);
		//		return false;
		//	}
		//	m_CopyQueue->SetName(L"Copy Queue");
		//	LOG_TRACE(LOG_TAG, "DX12 copy queue created");
		//}

		nvrhi::d3d12::DeviceDesc deviceDesc;
		//deviceDesc.errorCB = m_DeviceParams.messageCallback ? m_DeviceParams.messageCallback : &DefaultMessageCallback::GetInstance();
		deviceDesc.pDevice = m_Device12.Get();
		deviceDesc.pGraphicsCommandQueue = m_GraphicsQueue.Get();
		deviceDesc.pComputeCommandQueue = m_ComputeQueue.Get();
		deviceDesc.pCopyCommandQueue = m_CopyQueue.Get();
		//deviceDesc.logBufferLifetime = m_DeviceParams.logBufferLifetime;
		//deviceDesc.enableHeapDirectlyIndexed = false;

		m_NvrhiDevice = nvrhi::d3d12::createDevice(deviceDesc);

		//if (m_DeviceParams.enableNvrhiValidationLayer)
		if (false)
		{
			m_NvrhiDevice = nvrhi::validation::createValidationLayer(m_NvrhiDevice);
			LOG_TRACE(LOG_TAG, "NVRHI validation layer enabled");
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

		ZeroMemory(&m_SwapChainDesc, sizeof(m_SwapChainDesc));
		m_SwapChainDesc.Width = width;
		m_SwapChainDesc.Height = height;
		m_SwapChainDesc.SampleDesc.Count = 1;//m_DeviceParams.swapChainSampleCount;
		m_SwapChainDesc.SampleDesc.Quality = 0;
		m_SwapChainDesc.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;//m_DeviceParams.swapChainUsage;
		m_SwapChainDesc.BufferCount = 3;//m_DeviceParams.swapChainBufferCount;
		m_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		m_SwapChainDesc.Flags = 0;//m_DeviceParams.allowModeSwitch ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;

		m_SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
		if (SUCCEEDED(m_DxgiFactory2->QueryInterface(IID_PPV_ARGS(pDxgiFactory5.GetAddressOf()))))
		{
			BOOL supported = 0;
			if (SUCCEEDED(pDxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &supported, sizeof(supported))))
			{
				m_TearingSupported = (supported != 0);
			}
		}

		if (m_TearingSupported)
		{
			m_SwapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		}

		m_FullScreenDesc = {};
		m_FullScreenDesc.RefreshRate.Numerator = 0;//m_DeviceParams.refreshRate;
		m_FullScreenDesc.RefreshRate.Denominator = 1;
		m_FullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
		m_FullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		m_FullScreenDesc.Windowed = true;//!m_DeviceParams.startFullscreen;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapChain1;
		hr = m_DxgiFactory2->CreateSwapChainForHwnd(m_GraphicsQueue.Get(), hWnd, &m_SwapChainDesc, &m_FullScreenDesc, nullptr, &pSwapChain1);
		if (FAILED(hr))
		{
			LOG_ERROR(LOG_TAG, "CreateSwapChainForHwnd failed! Error code: {}", hr);
			return false;
		}

		hr = pSwapChain1->QueryInterface(IID_PPV_ARGS(m_SwapChain.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		if (!CreateRenderTargets())
		{
			LOG_ERROR(LOG_TAG, "Failed to create render targets from the back buffers!");
			return false;
		}

		hr = m_Device12->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_FrameFence.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		for (UINT bufferIndex = 0; bufferIndex < m_SwapChainDesc.BufferCount; bufferIndex++)
		{
			m_FrameFenceEvents.push_back(CreateEvent(nullptr, false, true, nullptr));
		}

		return true;
	}

	uint32_t DX12DeviceManager::GetCurrentBackBufferIndex()
	{
		return m_SwapChain->GetCurrentBackBufferIndex();
	}

	uint32_t DX12DeviceManager::GetBackBufferCount()
	{
		return m_SwapChainDesc.BufferCount;
	}

	bool DX12DeviceManager::CreateRenderTargets()
	{
		m_SwapChainBuffers.resize(m_SwapChainDesc.BufferCount);

		for (UINT n = 0; n < m_SwapChainDesc.BufferCount; n++)
		{
			const HRESULT hr = m_SwapChain->GetBuffer(n, IID_PPV_ARGS(m_SwapChainBuffers[n].nativeBuffer.GetAddressOf()));
			if (FAILED(hr))
			{
				return false;
			}

			nvrhi::TextureDesc textureDesc;
			textureDesc.width = m_SwapChainDesc.Width;
			textureDesc.height = m_SwapChainDesc.Height;
			textureDesc.sampleCount = m_SwapChainDesc.SampleDesc.Count;
			textureDesc.sampleQuality = m_SwapChainDesc.SampleDesc.Quality;
			textureDesc.format = nvrhi::Format::RGBA8_UNORM;
			textureDesc.debugName = "SwapChainBuffer";
			textureDesc.isRenderTarget = true;
			textureDesc.isUAV = false;
			textureDesc.initialState = nvrhi::ResourceStates::Present;
			textureDesc.keepInitialState = true;

			m_SwapChainBuffers[n].buffer = m_NvrhiDevice->createHandleForNativeTexture(nvrhi::ObjectTypes::D3D12_Resource, nvrhi::Object(m_SwapChainBuffers[n].nativeBuffer.Get()), textureDesc);
		}

		CreateFramebuffers();

		return true;
	}
	
	void DX12DeviceManager::DestroyRenderTargets()
	{
		DestroyFramebuffers();

		if (m_NvrhiDevice)
		{
			m_NvrhiDevice->waitForIdle();
			m_NvrhiDevice->runGarbageCollection();
		}

		for (auto e : m_FrameFenceEvents)
		{
			SetEvent(e);
		}

		m_SwapChainBuffers.clear();
	}

	void DX12DeviceManager::ResizeSwapChain()
	{
		DestroyRenderTargets();

		if (!m_NvrhiDevice) return;
		if (!m_SwapChain) return;

		const HRESULT hr = m_SwapChain->ResizeBuffers(
			m_SwapChainDesc.BufferCount,
			m_SwapChainDesc.Width,
			m_SwapChainDesc.Height,
			DXGI_FORMAT_UNKNOWN,
			m_SwapChainDesc.Flags);

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

	void DX12DeviceManager::CreateFramebuffers()
	{
		m_SwapChainFramebuffers.resize(m_SwapChainDesc.BufferCount);
		for (size_t i = 0; i < m_SwapChainDesc.BufferCount; i++)
		{
			nvrhi::FramebufferDesc framebufferDesc = nvrhi::FramebufferDesc().addColorAttachment(m_SwapChainBuffers[i].buffer);
			m_SwapChainFramebuffers[i] = m_NvrhiDevice->createFramebuffer(framebufferDesc);
		}
	}

	void DX12DeviceManager::DestroyFramebuffers()
	{
		m_SwapChainFramebuffers.clear();
	}
}
