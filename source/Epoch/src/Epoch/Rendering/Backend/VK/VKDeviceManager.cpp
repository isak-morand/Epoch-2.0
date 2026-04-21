#include "epch.h"
#include "VKDeviceManager.h"
#include "Epoch/Rendering/Backend/NVRHI/NVRHIMessageCallback.h"
#include "Epoch/Core/Window.h"

#include <nvrhi/vulkan.h>
#include <nvrhi/validation.h>
#include <vulkan/vulkan_win32.h>


VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace Epoch::RHI
{
#define LOG_TAG LogTags::Renderer

	namespace
	{
		std::vector<const char*> StringSetToVector(const std::unordered_set<std::string>& aSet)
		{
			std::vector<const char*> ret;
			for (const auto& s : aSet)
			{
				ret.push_back(s.c_str());
			}

			return ret;
		}

		VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
			vk::DebugReportFlagsEXT flags,
			vk::DebugReportObjectTypeEXT objType,
			uint64_t obj,
			size_t location,
			int32_t code,
			const char* layerPrefix,
			const char* msg,
			void* userData)
		{
			//const VKDeviceManager* manager = (const VKDeviceManager*)userData;
			//
			//if (manager)
			{
				//const auto& ignored = manager->GetDeviceParams().ignoredVulkanValidationMessageLocations;
				//const auto found = std::find(ignored.begin(), ignored.end(), location);
				//if (found != ignored.end())
				//	return VK_FALSE;

				if (location == 0x13365b2)
				{
					return VK_FALSE;
				}
			}

			LOG_WARN(LOG_TAG, "[Vulkan: location={} code={}, layerPrefix='{}'] {}", location, code, layerPrefix, msg);

			return VK_FALSE;
		}
	}

	VKDeviceManager::VKDeviceManager() = default;
	VKDeviceManager::~VKDeviceManager() = default;

	void VKDeviceManager::DestroyDeviceAndSwapChain()
	{
	}

	void VKDeviceManager::OnWindowResize(uint32_t aWidth, uint32_t aHeight)
	{
	}

	bool VKDeviceManager::BeginFrame()
	{
		return false;
	}

	bool VKDeviceManager::EndFrame()
	{
		return false;
	}

	std::shared_ptr<RHI::Buffer> VKDeviceManager::CreateBuffer(const RHI::BufferDesc& aDesc)
	{
		return std::shared_ptr<RHI::Buffer>();
	}

	std::shared_ptr<RHI::Texture> VKDeviceManager::CreateTexture(const RHI::TextureDesc& aDesc)
	{
		return std::shared_ptr<RHI::Texture>();
	}

	bool VKDeviceManager::CreateInstance()
	{
		if (m_RenderDesc.enableDebugRuntime)
		{
			enabledExtensions.instance.insert("VK_EXT_debug_report");
			enabledExtensions.layers.insert("VK_LAYER_KHRONOS_validation");
		}

		m_DynamicLoader = std::make_unique<VulkanDynamicLoader>();

		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = m_DynamicLoader->getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

		enabledExtensions.instance.insert("VK_KHR_surface");
		enabledExtensions.instance.insert("VK_KHR_win32_surface");

		std::unordered_set<std::string> requiredExtensions = enabledExtensions.instance;
		for (const auto& instanceExt : vk::enumerateInstanceExtensionProperties())
		{
			const std::string name = instanceExt.extensionName;
			if (optionalExtensions.instance.find(name) != optionalExtensions.instance.end())
			{
				enabledExtensions.instance.insert(name);
			}

			requiredExtensions.erase(name);
		}

		if (!requiredExtensions.empty())
		{
			std::stringstream ss;
			ss << "Cannot create a Vulkan instance because the following required extension(s) are not supported:";
			for (const auto& ext : requiredExtensions)
			{
				ss << std::endl << "  - " << ext;
			}

			LOG_ERROR(LOG_TAG, "{}", ss.str().c_str());
			return false;
		}

		LOG_TRACE(LOG_TAG, "Enabled Vulkan instance extensions:");
		for (const auto& ext : enabledExtensions.instance)
		{
			LOG_TRACE(LOG_TAG, "    {}", ext.c_str());
		}

		std::unordered_set<std::string> requiredLayers = enabledExtensions.layers;

		for (const auto& layer : vk::enumerateInstanceLayerProperties())
		{
			const std::string name = layer.layerName;
			if (optionalExtensions.layers.find(name) != optionalExtensions.layers.end())
			{
				enabledExtensions.layers.insert(name);
			}

			requiredLayers.erase(name);
		}

		if (!requiredLayers.empty())
		{
			std::stringstream ss;
			ss << "Cannot create a Vulkan instance because the following required layer(s) are not supported:";
			for (const auto& ext : requiredLayers)
			{
				ss << std::endl << "  - " << ext;
			}

			LOG_ERROR(LOG_TAG, "{}", ss.str().c_str());
			return false;
		}

		LOG_TRACE(LOG_TAG, "Enabled Vulkan layers:");
		for (const auto& layer : enabledExtensions.layers)
		{
			LOG_TRACE(LOG_TAG, "    {}", layer.c_str());
		}

		auto instanceExtVec = StringSetToVector(enabledExtensions.instance);
		auto layerVec = StringSetToVector(enabledExtensions.layers);

		auto applicationInfo = vk::ApplicationInfo();

		vk::Result res = vk::enumerateInstanceVersion(&applicationInfo.apiVersion);

		if (res != vk::Result::eSuccess)
		{
			LOG_ERROR(LOG_TAG, "Call to vkEnumerateInstanceVersion failed! Error: {}", nvrhi::vulkan::resultToString(VkResult(res)));
			return false;
		}

		const uint32_t minimumVulkanVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);

		if (applicationInfo.apiVersion < minimumVulkanVersion)
		{
			LOG_ERROR(LOG_TAG, "The Vulkan API version supported on the system ({}.{}.{}) is too low, at least {}.{}.{} is required.",
				VK_API_VERSION_MAJOR(applicationInfo.apiVersion), VK_API_VERSION_MINOR(applicationInfo.apiVersion), VK_API_VERSION_PATCH(applicationInfo.apiVersion),
				VK_API_VERSION_MAJOR(minimumVulkanVersion), VK_API_VERSION_MINOR(minimumVulkanVersion), VK_API_VERSION_PATCH(minimumVulkanVersion));
			return false;
		}

		// Spec says: A non-zero variant indicates the API is a variant of the Vulkan API and applications will typically need to be modified to run against it.
		if (VK_API_VERSION_VARIANT(applicationInfo.apiVersion) != 0)
		{
			LOG_ERROR(LOG_TAG, "The Vulkan API supported on the system uses an unexpected variant: {}.", VK_API_VERSION_VARIANT(applicationInfo.apiVersion));
			return false;
		}

		vk::InstanceCreateInfo info = vk::InstanceCreateInfo()
			.setEnabledLayerCount(uint32_t(layerVec.size()))
			.setPpEnabledLayerNames(layerVec.data())
			.setEnabledExtensionCount(uint32_t(instanceExtVec.size()))
			.setPpEnabledExtensionNames(instanceExtVec.data())
			.setPApplicationInfo(&applicationInfo);

		res = vk::createInstance(&info, nullptr, &m_VulkanInstance);
		if (res != vk::Result::eSuccess)
		{
			LOG_ERROR(LOG_TAG, "Failed to create a Vulkan instance! Error: {}", nvrhi::vulkan::resultToString(VkResult(res)));
			return false;
		}

		VULKAN_HPP_DEFAULT_DISPATCHER.init(m_VulkanInstance);

		return true;
	}

	bool VKDeviceManager::CreateDevice(Window* aWindow)
	{
		if (m_RenderDesc.enableDebugRuntime)
		{
			InstallDebugCallback();
		}

		if (!CreateWindowSurface(aWindow))
		{
			return false;
		}

		if (!PickPhysicalDevice(aWindow))
		{
			return false;
		}

		if (!FindQueueFamilies(m_VulkanPhysicalDevice))
		{
			return false;
		}

		if (!CreateDeviceInternal())
		{
			return false;
		}

		auto vecInstanceExt = StringSetToVector(enabledExtensions.instance);
		auto vecLayers = StringSetToVector(enabledExtensions.layers);
		auto vecDeviceExt = StringSetToVector(enabledExtensions.device);

		nvrhi::vulkan::DeviceDesc deviceDesc;
		deviceDesc.errorCB = &NVRHIMessageCallback::GetInstance();
		deviceDesc.instance = m_VulkanInstance;
		deviceDesc.physicalDevice = m_VulkanPhysicalDevice;
		deviceDesc.device = m_VulkanDevice;
		deviceDesc.graphicsQueue = m_GraphicsQueue;
		deviceDesc.graphicsQueueIndex = m_GraphicsQueueFamily;
		//if (m_DeviceParams.enableComputeQueue)
		//{
		//	deviceDesc.computeQueue = m_ComputeQueue;
		//	deviceDesc.computeQueueIndex = m_ComputeQueueFamily;
		//}
		//if (m_DeviceParams.enableCopyQueue)
		//{
		//	deviceDesc.transferQueue = m_TransferQueue;
		//	deviceDesc.transferQueueIndex = m_TransferQueueFamily;
		//}
		deviceDesc.instanceExtensions = vecInstanceExt.data();
		deviceDesc.numInstanceExtensions = vecInstanceExt.size();
		deviceDesc.deviceExtensions = vecDeviceExt.data();
		deviceDesc.numDeviceExtensions = vecDeviceExt.size();

		m_NvrhiDevice = nvrhi::vulkan::createDevice(deviceDesc);

		if (m_RenderDesc.enableGPUValidation)
		{
			m_NvrhiDevice = nvrhi::validation::createValidationLayer(m_NvrhiDevice);
		}

		return true;
	}

	bool VKDeviceManager::CreateSwapChain(Window* aWindow)
	{
		return false;
	}

	uint32_t VKDeviceManager::GetCurrentBackBufferIndex()
	{
		return 0;
	}

	uint32_t VKDeviceManager::GetBackBufferCount()
	{
		return 0;
	}

	void VKDeviceManager::InstallDebugCallback()
	{
		auto info = vk::DebugReportCallbackCreateInfoEXT()
			.setFlags(vk::DebugReportFlagBitsEXT::eError |
				vk::DebugReportFlagBitsEXT::eWarning |
				//vk::DebugReportFlagBitsEXT::eInformation |
				vk::DebugReportFlagBitsEXT::ePerformanceWarning)
			.setPfnCallback(VulkanDebugCallback)
			.setPUserData(this);

		vk::Result res = m_VulkanInstance.createDebugReportCallbackEXT(&info, nullptr, &m_DebugReportCallback);
		assert(res == vk::Result::eSuccess);
		(void)res;
	}
	
	bool VKDeviceManager::CreateWindowSurface(Window* aWindow)
	{
		vk::Win32SurfaceCreateInfoKHR createInfo{};
		createInfo.hinstance = GetModuleHandle(nullptr);;
		createInfo.hwnd = (HWND)aWindow->GetNativeWindow();

		m_WindowSurface = m_VulkanInstance.createWin32SurfaceKHR(createInfo);
		return m_WindowSurface;
	}

	bool VKDeviceManager::PickPhysicalDevice(Window* aWindow)
	{
		VkFormat requestedFormat = VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
		vk::Extent2D requestedExtent(aWindow->GetWidth(), aWindow->GetHeight());

		auto devices = m_VulkanInstance.enumeratePhysicalDevices();

		uint32_t firstDevice = 0;
		uint32_t lastDevice = int(devices.size()) - 1;

		std::stringstream errorStream;
		errorStream << "Cannot find a Vulkan device that supports all the required extensions and properties.";

		std::vector<vk::PhysicalDevice> discreteGPUs;
		std::vector<vk::PhysicalDevice> otherGPUs;
		for (uint32_t deviceIndex = firstDevice; deviceIndex <= lastDevice; ++deviceIndex)
		{
			vk::PhysicalDevice const& dev = devices[deviceIndex];
			vk::PhysicalDeviceProperties prop = dev.getProperties();

			errorStream << std::endl << prop.deviceName.data() << ":";

			std::unordered_set<std::string> requiredExtensions = enabledExtensions.device;
			auto deviceExtensions = dev.enumerateDeviceExtensionProperties();
			for (const auto& ext : deviceExtensions)
			{
				requiredExtensions.erase(std::string(ext.extensionName.data()));
			}

			bool deviceIsGood = true;

			if (!requiredExtensions.empty())
			{
				for (const auto& ext : requiredExtensions)
				{
					errorStream << std::endl << "  - missing " << ext;
				}
				deviceIsGood = false;
			}

			vk::PhysicalDeviceFeatures2 deviceFeatures2{};
			vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
			deviceFeatures2.pNext = &dynamicRenderingFeatures;

			dev.getFeatures2(&deviceFeatures2);
			if (!deviceFeatures2.features.samplerAnisotropy)
			{
				errorStream << std::endl << "  - does not support samplerAnisotropy";
				deviceIsGood = false;
			}
			if (!deviceFeatures2.features.textureCompressionBC)
			{
				errorStream << std::endl << "  - does not support textureCompressionBC";
				deviceIsGood = false;
			}
			if (!dynamicRenderingFeatures.dynamicRendering)
			{
				errorStream << std::endl << "  - does not support dynamicRendering";
				deviceIsGood = false;
			}

			if (!FindQueueFamilies(dev))
			{
				errorStream << std::endl << "  - does not support the necessary queue types";
				deviceIsGood = false;
			}

			if (deviceIsGood && m_WindowSurface)
			{
				bool surfaceSupported = dev.getSurfaceSupportKHR(m_PresentQueueFamily, m_WindowSurface);
				if (!surfaceSupported)
				{
					errorStream << std::endl << "  - does not support the window surface";
					deviceIsGood = false;
				}
				else
				{
					auto surfaceCaps = dev.getSurfaceCapabilitiesKHR(m_WindowSurface);
					auto surfaceFmts = dev.getSurfaceFormatsKHR(m_WindowSurface);

					if (surfaceCaps.minImageCount > m_RenderDesc.swapChainBufferCount ||
						(surfaceCaps.maxImageCount < m_RenderDesc.swapChainBufferCount && surfaceCaps.maxImageCount > 0))
					{
						errorStream << std::endl << "  - cannot support the requested swap chain image count:";
						errorStream << " requested " << m_RenderDesc.swapChainBufferCount << ", available " << surfaceCaps.minImageCount << " - " << surfaceCaps.maxImageCount;
						deviceIsGood = false;
					}

					if (surfaceCaps.minImageExtent.width > requestedExtent.width ||
						surfaceCaps.minImageExtent.height > requestedExtent.height ||
						surfaceCaps.maxImageExtent.width < requestedExtent.width ||
						surfaceCaps.maxImageExtent.height < requestedExtent.height)
					{
						errorStream << std::endl << "  - cannot support the requested swap chain size:";
						errorStream << " requested " << requestedExtent.width << "x" << requestedExtent.height << ", ";
						errorStream << " available " << surfaceCaps.minImageExtent.width << "x" << surfaceCaps.minImageExtent.height;
						errorStream << " - " << surfaceCaps.maxImageExtent.width << "x" << surfaceCaps.maxImageExtent.height;
						deviceIsGood = false;
					}

					bool surfaceFormatPresent = false;
					for (const vk::SurfaceFormatKHR& surfaceFmt : surfaceFmts)
					{
						if (surfaceFmt.format == vk::Format(requestedFormat))
						{
							surfaceFormatPresent = true;
							break;
						}
					}

					if (!surfaceFormatPresent)
					{
						errorStream << std::endl << "  - does not support the requested swap chain format";
						deviceIsGood = false;
					}

					uint32_t canPresent = dev.getSurfaceSupportKHR(m_GraphicsQueueFamily, m_WindowSurface);
					if (!canPresent)
					{
						errorStream << std::endl << "  - cannot present";
						deviceIsGood = false;
					}
				}
			}

			if (!deviceIsGood) continue;

			if (prop.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			{
				discreteGPUs.push_back(dev);
			}
			else
			{
				otherGPUs.push_back(dev);
			}
		}

		if (!discreteGPUs.empty())
		{
			m_VulkanPhysicalDevice = discreteGPUs[0];
			return true;
		}

		if (!otherGPUs.empty())
		{
			m_VulkanPhysicalDevice = otherGPUs[0];
			return true;
		}

		LOG_ERROR(LOG_TAG, "{}", errorStream.str());

		return false;
	}

	bool VKDeviceManager::FindQueueFamilies(vk::PhysicalDevice aPhysicalDevice)
	{
		auto props = aPhysicalDevice.getQueueFamilyProperties();

		for (uint32_t i = 0; i < uint32_t(props.size()); i++)
		{
			const auto& queueFamily = props[i];

			if (m_GraphicsQueueFamily == -1)
			{
				if (queueFamily.queueCount > 0 &&
					(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics))
				{
					m_GraphicsQueueFamily = i;
				}
			}

			if (m_ComputeQueueFamily == -1)
			{
				if (queueFamily.queueCount > 0 &&
					(queueFamily.queueFlags & vk::QueueFlagBits::eCompute) &&
					!(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics))
				{
					m_ComputeQueueFamily = i;
				}
			}

			if (m_TransferQueueFamily == -1)
			{
				if (queueFamily.queueCount > 0 &&
					(queueFamily.queueFlags & vk::QueueFlagBits::eTransfer) &&
					!(queueFamily.queueFlags & vk::QueueFlagBits::eCompute) &&
					!(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics))
				{
					m_TransferQueueFamily = i;
				}
			}

			if (m_PresentQueueFamily == -1)
			{
				if (queueFamily.queueCount > 0 &&
					aPhysicalDevice.getWin32PresentationSupportKHR(i))
				{
					m_PresentQueueFamily = i;
				}
			}
		}

		if (m_GraphicsQueueFamily == -1 ||
			m_PresentQueueFamily == -1// ||
			/*(m_ComputeQueueFamily == -1 && m_RenderDesc.enableComputeQueue) || */
			/*(m_TransferQueueFamily == -1 && m_RenderDesc.enableCopyQueue)*/)
		{
			return false;
		}

		return true;
	}

	bool VKDeviceManager::CreateDeviceInternal()
	{
		auto deviceExtensions = m_VulkanPhysicalDevice.enumerateDeviceExtensionProperties();
		for (const auto& ext : deviceExtensions)
		{
			const std::string name = ext.extensionName;
			if (optionalExtensions.device.find(name) != optionalExtensions.device.end())
			{
				enabledExtensions.device.insert(name);
			}
		}

		const auto& extSet = enabledExtensions.device;
		LOG_TRACE(LOG_TAG, "Enabled Vulkan device extensions:");
		for (const auto& ext : extSet)
		{
			LOG_TRACE(LOG_TAG, "    {}", ext);
		}
		bool maintenance4Supported = extSet.contains(VK_KHR_MAINTENANCE_4_EXTENSION_NAME);
		bool synchronization2Supported = extSet.contains(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);

#define APPEND_EXTENSION(condition, desc) if (condition) { (desc).pNext = pNext; pNext = &(desc); }
		void* pNext = nullptr;

		vk::PhysicalDeviceFeatures2 physicalDeviceFeatures2;
		auto maintenance4Features = vk::PhysicalDeviceMaintenance4Features();
		auto synchronization2Features = vk::PhysicalDeviceSynchronization2Features();

		APPEND_EXTENSION(maintenance4Supported, maintenance4Features);
		APPEND_EXTENSION(synchronization2Supported, synchronization2Features);

		physicalDeviceFeatures2.pNext = pNext;
		m_VulkanPhysicalDevice.getFeatures2(&physicalDeviceFeatures2);

		auto vulkan13features = vk::PhysicalDeviceVulkan13Features()
			.setDynamicRendering(true)
			.setSynchronization2(synchronization2Features.synchronization2)
			.setMaintenance4(maintenance4Features.maintenance4);

		auto dynamicRenderingFeatures = vk::PhysicalDeviceDynamicRenderingFeatures().setDynamicRendering(true);

		const vk::PhysicalDeviceProperties physicalDeviceProperties = m_VulkanPhysicalDevice.getProperties();
		m_AdaptorName = std::string(physicalDeviceProperties.deviceName.data());

		pNext = nullptr;
		APPEND_EXTENSION(physicalDeviceProperties.apiVersion >= VK_API_VERSION_1_3, vulkan13features);
		APPEND_EXTENSION(physicalDeviceProperties.apiVersion < VK_API_VERSION_1_3 && maintenance4Supported, maintenance4Features);
		APPEND_EXTENSION(physicalDeviceProperties.apiVersion < VK_API_VERSION_1_3, dynamicRenderingFeatures);

#undef APPEND_EXTENSION

		auto deviceFeatures = vk::PhysicalDeviceFeatures()
			.setShaderImageGatherExtended(true)
			.setSamplerAnisotropy(true)
			.setTessellationShader(true)
			.setTextureCompressionBC(true)
			.setGeometryShader(true)
			.setImageCubeArray(true)
			.setShaderInt16(true)
			.setFillModeNonSolid(true)
			.setFragmentStoresAndAtomics(true)
			.setDualSrcBlend(true)
			.setVertexPipelineStoresAndAtomics(true)
			.setShaderInt64(true)
			.setShaderStorageImageWriteWithoutFormat(true)
			.setShaderStorageImageReadWithoutFormat(true);

		// Add a Vulkan 1.1 structure with default settings to make it easier for apps to modify them
		auto vulkan11features = vk::PhysicalDeviceVulkan11Features()
			.setStorageBuffer16BitAccess(true)
			.setPNext(pNext);

		auto vulkan12features = vk::PhysicalDeviceVulkan12Features()
			.setDescriptorIndexing(true)
			.setRuntimeDescriptorArray(true)
			.setDescriptorBindingPartiallyBound(true)
			.setDescriptorBindingVariableDescriptorCount(true)
			.setTimelineSemaphore(true)
			.setShaderSampledImageArrayNonUniformIndexing(true)
			.setShaderSubgroupExtendedTypes(true)
			.setScalarBlockLayout(true)
			.setPNext(&vulkan11features);

		auto extVec = StringSetToVector(enabledExtensions.device);

		std::unordered_set<int32_t> uniqueQueueFamilies = { m_GraphicsQueueFamily };

		float priority = 1.f;
		std::vector<vk::DeviceQueueCreateInfo> queueDesc;
		queueDesc.reserve(uniqueQueueFamilies.size());
		for (int32_t queueFamily : uniqueQueueFamilies)
		{
			queueDesc.push_back(vk::DeviceQueueCreateInfo()
				.setQueueFamilyIndex(queueFamily)
				.setQueueCount(1)
				.setPQueuePriorities(&priority));
		}

		auto deviceDesc = vk::DeviceCreateInfo()
			.setPQueueCreateInfos(queueDesc.data())
			.setQueueCreateInfoCount(uint32_t(queueDesc.size()))
			.setPEnabledFeatures(&deviceFeatures)
			.setEnabledExtensionCount(uint32_t(extVec.size()))
			.setPpEnabledExtensionNames(extVec.data())
			.setPNext(&vulkan12features);

		const vk::Result res = m_VulkanPhysicalDevice.createDevice(&deviceDesc, nullptr, &m_VulkanDevice);
		if (res != vk::Result::eSuccess)
		{
			LOG_ERROR(LOG_TAG, "Failed to create a Vulkan physical device! Error: {}", nvrhi::vulkan::resultToString(VkResult(res)));
			return false;
		}

		m_VulkanDevice.getQueue(m_GraphicsQueueFamily, 0, &m_GraphicsQueue);

		VULKAN_HPP_DEFAULT_DISPATCHER.init(m_VulkanDevice);

		LOG_TRACE(LOG_TAG, "Created Vulkan device: {}", m_AdaptorName);

		return true;
	}
}
