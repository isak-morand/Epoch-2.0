local NVRHI_DIR = "%{wks.location}/vendor/NVRHI"

project "NVRHI-D3D12"
	kind "StaticLib"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		NVRHI_DIR .. "/include/nvrhi/d3d12.h",

		NVRHI_DIR .. "/src/common/dxgi-format.h",
		NVRHI_DIR .. "/src/common/dxgi-format.cpp",
		NVRHI_DIR .. "/src/common/versioning.h",

		NVRHI_DIR .. "/src/d3d12/**.h",
		NVRHI_DIR .. "/src/d3d12/**.cpp",
	}
	
	defines
	{
		--"NVRHI_WITH_RTXMU",
		"NOMINMAX"
	}

	includedirs
	{
		NVRHI_DIR .. "/include",
		--"/rtxmu/include",

		NVRHI_DIR .. "/thirdparty/DirectX-Headers/include",
	}
	
    links { "d3d12", "dxgi", "dxguid", "d3dcompiler" }

project "NVRHI-Vulkan"
	kind "StaticLib"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		NVRHI_DIR .. "/include/nvrhi/vulkan.h",
		NVRHI_DIR .. "/src/vulkan/**.h",
		NVRHI_DIR .. "/src/vulkan/**.cpp",
	}

	VULKAN_SDK = os.getenv("VULKAN_SDK")

	includedirs
	{
		NVRHI_DIR .. "/include",
		NVRHI_DIR .. "/rtxmu/include",

		"%{VULKAN_SDK}/Include",
		--NVRHI_DIR .. "/thirdparty/Vulkan-Headers/include",
	}
	
	libdirs
	{
		"%{VULKAN_SDK}/Lib",
	}
	
    links { "vulkan-1" }
	
	defines
	{
		--"NVRHI_WITH_RTXMU=1",
		"VK_USE_PLATFORM_WIN32_KHR",
		"NOMINMAX",
		"VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1",
	}

project "NVRHI"
    kind "StaticLib"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    
	files
	{
		NVRHI_DIR .. "/include/nvrhi/nvrhi.h",
		NVRHI_DIR .. "/include/nvrhi/utils.h",

		NVRHI_DIR .. "/include/nvrhi/common/**.h",
		NVRHI_DIR .. "/src/common/**.cpp",

		NVRHI_DIR .. "/src/validation/**.h",
		NVRHI_DIR .. "/src/validation/**.cpp",

		NVRHI_DIR .. "/tools/nvrhi.natvis"
	}

    includedirs
    {
		NVRHI_DIR .. "/include",
        --NVRHI_DIR .. "/rtxmu/include",

		NVRHI_DIR .. "/thirdparty/DirectX-Headers/include",
    }
	
    links { "NVRHI-D3D12", "NVRHI-Vulkan" }

	defines
	{
		--"NVRHI_WITH_RTXMU=1",
		"NOMINMAX",
	}
