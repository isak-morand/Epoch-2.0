#pragma once
#include <queue>
#include <unordered_set>
#include "Epoch/Rendering/DeviceManager.h"

#ifndef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#endif
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

namespace Epoch::RHI
{
	class VKDeviceManager : public DeviceManager
	{
	public:
		VKDeviceManager();
		~VKDeviceManager() override;

		void DestroyDeviceAndSwapChain() override;

		void OnWindowResize(uint32_t aWidth, uint32_t aHeight) override;

		bool BeginFrame() override;
		bool EndFrame() override;

		nvrhi::IDevice* GetDevice() override { return nullptr; }
		nvrhi::IFramebuffer* GetCurrentFramebuffer() override { return nullptr; }
		nvrhi::ITexture* GetCurrentFramebufferImage() override { return nullptr; }

		std::shared_ptr<RHI::Buffer> CreateBuffer(const RHI::BufferDesc& aDesc) override;
		std::shared_ptr<RHI::Texture> CreateTexture(const RHI::TextureDesc& aDesc) override;

	protected:
		bool CreateInstance() override;
		bool CreateDevice(Window* aWindow) override;
		bool CreateSwapChain(Window* aWindow) override;

		uint32_t GetCurrentBackBufferIndex() override;
		uint32_t GetBackBufferCount() override;

	private:
		bool CreateRenderTargets();
		void DestroyRenderTargets();
		void ResizeSwapChain(uint32_t aWidth, uint32_t aHeight);

		void InstallDebugCallback();
		bool CreateWindowSurface(Window* aWindow);
		bool PickPhysicalDevice(Window* aWindow);
		bool FindQueueFamilies(vk::PhysicalDevice aPhysicalDevice);
		bool CreateDeviceInternal();

	private:
		struct VulkanExtensionSet
		{
			std::unordered_set<std::string> instance;
			std::unordered_set<std::string> layers;
			std::unordered_set<std::string> device;
		};

		VulkanExtensionSet enabledExtensions =
		{
			// instance
			{
				VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
			},
			// layers
			{ },
			// device
			{
				VK_KHR_MAINTENANCE1_EXTENSION_NAME,
				VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			},
		};

		VulkanExtensionSet optionalExtensions =
		{
			// instance
			{
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME, 
				VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME,
			},
			// layers
			{ },
			// device
			{
				VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
				VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
				VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
			},
		};

		vk::Instance m_VulkanInstance;
		vk::DebugReportCallbackEXT m_DebugReportCallback;

		vk::PhysicalDevice m_VulkanPhysicalDevice;
		int32_t m_GraphicsQueueFamily = -1;
		int32_t m_ComputeQueueFamily = -1;
		int32_t m_TransferQueueFamily = -1;
		int32_t m_PresentQueueFamily = -1;

		vk::Device m_VulkanDevice;
		vk::Queue m_GraphicsQueue;
		vk::Queue m_ComputeQueue;
		vk::Queue m_TransferQueue;
		vk::Queue m_PresentQueue;

		vk::SurfaceKHR m_WindowSurface;

		vk::SurfaceFormatKHR m_SwapChainFormat;
		vk::SwapchainKHR m_SwapChain;

		std::vector<vk::Image> m_SwapChainBuffers;
		std::vector<nvrhi::TextureHandle> m_RHISwapChainBuffers;
		uint32_t m_SwapChainIndex = uint32_t(-1);

		nvrhi::DeviceHandle m_NvrhiDevice;

		//bool m_BufferDeviceAddressSupported = false;

#if VK_HEADER_VERSION >= 301
		typedef vk::detail::DynamicLoader VulkanDynamicLoader;
#else
		typedef vk::DynamicLoader VulkanDynamicLoader;
#endif

		std::unique_ptr<VulkanDynamicLoader> m_DynamicLoader;
	};
}
