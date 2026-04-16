#pragma once
#include "RendererDesc.h"

namespace Epoch
{
	class Window;

	namespace RHI
	{
		class DeviceManager;
	}

	class Renderer
	{
	public:
		Renderer(Window* aWindow, const RendererDesc& aDesc);
		~Renderer();

		void OnWindowResize(uint32_t aWidth, uint32_t aHeight);

		void Render();

		const RHI::DeviceManager* GetDeviceManager() const { return m_DeviceManager.get(); }

		RHI::API GetAPI() const;

		float GetAverageFrameTime() const { return m_AverageFrameTime; }

	private:
		void UpdateAverageFrameTime();

	private:
		std::unique_ptr<RHI::DeviceManager> m_DeviceManager;

		float m_AverageFrameTime = 0.0f;
		float m_AverageTimeUpdateInterval = 0.5f;
		float m_FrameTimeSum = 0.0f;
		uint32_t m_NumberOfAccumulatedFrames = 0u;
	};
}
