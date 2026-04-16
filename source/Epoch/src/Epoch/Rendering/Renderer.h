#pragma once
#include "RendererDesc.h"

namespace Epoch
{
	class Window;
	class DeviceManager;

	class Renderer
	{
	public:
		Renderer(Window* aWindow, const RendererDesc& aDesc);
		~Renderer();

		void OnWindowResize(uint32_t aWidth, uint32_t aHeight);

		void Render();

		const DeviceManager* GetDeviceManager() const { return m_DeviceManager.get(); }

		GraphicsAPI GetAPI() const;

		float GetAverageFrameTime() const { return m_AverageFrameTime; }

	private:
		void UpdateAverageFrameTime();

	private:
		std::unique_ptr<DeviceManager> m_DeviceManager;

		float m_AverageFrameTime = 0.0;
		float m_AverageTimeUpdateInterval = 0.5;
		float m_FrameTimeSum = 0.0;
		uint32_t m_NumberOfAccumulatedFrames = 0;
	};
}
