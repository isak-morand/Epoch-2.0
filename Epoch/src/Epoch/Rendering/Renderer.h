#pragma once
#include "DeviceManager.h"

namespace Epoch
{
	class Window;

	class Renderer
	{
	public:
		Renderer(Window* aWindow, const RendererDesc& aDesc);
		~Renderer();

		void OnWindowResize(uint32_t aWidth, uint32_t aHeight);

		void Render();

	private:
		std::unique_ptr<DeviceManager> myDeviceManager;
	};
}
