#include "epch.h"
#include "Renderer.h"

namespace Epoch
{
	Renderer::Renderer(Window* aWindow, const RendererDesc& aDesc)
	{
		m_DeviceManager = DeviceManager::Create(aDesc.graphicsAPI);
		m_DeviceManager->CreateDeviceAndSwapChain(aWindow);
	}

	Renderer::~Renderer()
	{
		m_DeviceManager->DestroyDeviceAndSwapChain();
	}

	void Renderer::OnWindowResize(uint32_t aWidth, uint32_t aHeight)
	{
		m_DeviceManager->ResizeSwapChain(aWidth, aHeight);
	}

	void Renderer::Render()
	{
		m_DeviceManager->BeginFrame();

		m_DeviceManager->Render();

		m_DeviceManager->EndFrame();
	}
}
