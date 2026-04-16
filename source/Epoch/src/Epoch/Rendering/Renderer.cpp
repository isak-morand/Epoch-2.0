#include "epch.h"
#include "Renderer.h"
#include "DeviceManager.h"

#include "Epoch/Rendering/ShaderCompiler.h"
#include <nvrhi/nvrhi.h>

namespace Epoch
{
	static nvrhi::CommandListHandle				m_CommandList;
	static nvrhi::ShaderHandle					m_VertexShader;
	static nvrhi::ShaderHandle					m_PixelShader;
	static nvrhi::GraphicsPipelineHandle		m_Pipeline;

	Renderer::Renderer(Window* aWindow, const RendererDesc& aDesc)
	{
		m_DeviceManager = RHI::DeviceManager::Create(aDesc.graphicsAPI);
		EPOCH_VERIFY(m_DeviceManager, "Failed to create device manager!");
		EPOCH_VERIFY(m_DeviceManager->CreateDeviceAndSwapChain(aDesc, aWindow), "Failed to create device and or swap chain!");

		auto device = m_DeviceManager->GetDevice();

		m_CommandList = device->createCommandList();

		ShaderCompiler shaderCompiler;
		std::string shaderSource = R"(
				struct VSOut
				{
					float4 position : SV_Position;
				};
				
				VSOut vert(uint vertexID : SV_VertexID)
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

				float4 frag() : SV_Target
				{
					return float4(0.2, 0.8, 0.3, 1.0);
				})";

		{
			ShaderCompileInput vsIn;
			vsIn.source = shaderSource;
			vsIn.entryPoint = "vert";
			vsIn.stage = ShaderStage::Vertex;

			ShaderCompileOutput vsOut;
			shaderCompiler.Compile(vsIn, vsOut);

			nvrhi::ShaderDesc desc;
			desc.setShaderType(nvrhi::ShaderType::Vertex);

			m_VertexShader = device->createShader(desc, vsOut.bytecode.data(), vsOut.bytecode.size());
		}

		{
			ShaderCompileInput psIn;
			psIn.source = shaderSource;
			psIn.entryPoint = "frag";
			psIn.stage = ShaderStage::Pixel;

			ShaderCompileOutput psOut;
			shaderCompiler.Compile(psIn, psOut);

			nvrhi::ShaderDesc desc;
			desc.setShaderType(nvrhi::ShaderType::Pixel);

			m_PixelShader = device->createShader(desc, psOut.bytecode.data(), psOut.bytecode.size());
		}

		nvrhi::GraphicsPipelineDesc pipelineDesc;
		pipelineDesc.VS = m_VertexShader;
		pipelineDesc.PS = m_PixelShader;
		pipelineDesc.renderState.depthStencilState.depthTestEnable = false;

		nvrhi::FramebufferInfo fbInfo;
		fbInfo.addColorFormat(nvrhi::Format::RGBA8_UNORM);

		m_Pipeline = device->createGraphicsPipeline(pipelineDesc, fbInfo);
	}

	Renderer::~Renderer()
	{
		m_Pipeline = nullptr;

		m_DeviceManager->DestroyDeviceAndSwapChain();
	}

	void Renderer::OnWindowResize(uint32_t aWidth, uint32_t aHeight)
	{
		m_DeviceManager->OnWindowResize(aWidth, aHeight);
	}

	void Renderer::Render()
	{
		m_DeviceManager->BeginFrame();


		auto device = m_DeviceManager->GetDevice();
		auto fb = m_DeviceManager->GetCurrentFramebuffer();

		m_CommandList->open();
		m_CommandList->clearTextureFloat(m_DeviceManager->GetCurrentFramebufferImage(), nvrhi::AllSubresources, nvrhi::Color(0, 0, 0, 0));

		nvrhi::GraphicsState state;
		state.pipeline = m_Pipeline;
		state.framebuffer = fb;
		state.viewport.addViewportAndScissorRect(fb->getFramebufferInfo().getViewport());

		m_CommandList->setGraphicsState(state);

		m_CommandList->draw({ 3 });

		m_CommandList->close();

		device->executeCommandList(m_CommandList);


		m_DeviceManager->EndFrame();

		UpdateAverageFrameTime();
	}

	RHI::API Renderer::GetAPI() const
	{
		return m_DeviceManager->GetAPI();
	}

	void Renderer::UpdateAverageFrameTime()
	{
		static std::chrono::time_point<std::chrono::high_resolution_clock> lastTime = std::chrono::high_resolution_clock::now();
		std::chrono::time_point<std::chrono::high_resolution_clock> time = std::chrono::high_resolution_clock::now();
		float elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(time - lastTime).count() * 0.001f * 0.001f;
		lastTime = time;

		m_FrameTimeSum += elapsedTime;
		++m_NumberOfAccumulatedFrames;

		if (m_FrameTimeSum > m_AverageTimeUpdateInterval && m_NumberOfAccumulatedFrames > 0)
		{
			m_AverageFrameTime = m_FrameTimeSum / m_NumberOfAccumulatedFrames;
			m_NumberOfAccumulatedFrames = 0;
			m_FrameTimeSum = 0.0;
		}
	}
}
