#include "epch.h"
#include "Renderer.h"

namespace Epoch
{
	Renderer::Renderer(Window* aWindow, const RendererDesc& aDesc)
	{
		myDeviceManager = DeviceManager::Create(aDesc.API);
		myDeviceManager->CreateDeviceAndSwapChain(aWindow);
	}

	Renderer::~Renderer()
	{
		myDeviceManager->DestroyDeviceAndSwapChain();
	}

	void Renderer::OnWindowResize(uint32_t aWidth, uint32_t aHeight)
	{
		myDeviceManager->ResizeSwapChain(aWidth, aHeight);
	}

	void Renderer::Render()
	{
		myDeviceManager->BeginFrame();

		myDeviceManager->Render();

		myDeviceManager->EndFrame();
	}
}
