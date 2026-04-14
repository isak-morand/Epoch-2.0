#include "epch.h"
#include "Renderer.h"
#include "DeviceManager.h"

namespace Epoch
{
	Renderer::Renderer(Window* aWindow, const RendererDesc& aDesc)
	{
		m_DeviceManager = DeviceManager::Create(aDesc.graphicsAPI);
		EPOCH_VERIFY(m_DeviceManager, "Failed to create device manager!");
		EPOCH_VERIFY(m_DeviceManager->CreateDeviceAndSwapChain(aDesc, aWindow), "Failed to create device and or swap chain!");
	}

	Renderer::~Renderer()
	{
		m_DeviceManager->DestroyDeviceAndSwapChain();
	}

	void Renderer::OnWindowResize(uint32_t aWidth, uint32_t aHeight)
	{
		m_DeviceManager->OnWindowResize(aWidth, aHeight);
	}

	void Renderer::Render()
	{
		m_DeviceManager->BeginFrame();
		m_DeviceManager->Render();
		m_DeviceManager->EndFrame();

		UpdateAverageFrameTime();
	}

	void Renderer::UpdateAverageFrameTime()
	{
		static std::chrono::time_point<std::chrono::high_resolution_clock> lastTime = std::chrono::high_resolution_clock::now();
		std::chrono::time_point<std::chrono::high_resolution_clock> time = std::chrono::high_resolution_clock::now();
		float elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(time - lastTime).count() * 0.001f * 0.001f;
		lastTime = time;

		m_FrameTimeSum += elapsedTime;
		m_NumberOfAccumulatedFrames += 1;

		if (m_FrameTimeSum > m_AverageTimeUpdateInterval && m_NumberOfAccumulatedFrames > 0)
		{
			m_AverageFrameTime = m_FrameTimeSum / m_NumberOfAccumulatedFrames;
			m_NumberOfAccumulatedFrames = 0;
			m_FrameTimeSum = 0.0;
		}
	}
}
