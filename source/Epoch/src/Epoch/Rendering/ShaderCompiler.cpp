#include "epch.h"
#include "ShaderCompiler.h"

#ifdef EPOCH_PLATFORM_WINDOWS
#include <windows.h>
#endif

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <dxcapi.h>

namespace Epoch
{
#define LOG_TAG LogTags::Renderer

	namespace
	{
		static const wchar_t* StageToProfile(ShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage::Vertex:	return L"vs_6_0";
			case ShaderStage::Pixel:	return L"ps_6_0";
			case ShaderStage::Compute:	return L"cs_6_0";
			}
			return L"Unknown";
		}
	}

	bool ShaderCompiler::Compile(const ShaderCompileInput& aInput, ShaderCompileOutput& outOutput)
	{
		outOutput = {};
		outOutput.stage = aInput.stage;

		ComPtr<IDxcUtils> utils;
		ComPtr<IDxcCompiler3> compiler;

		if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils))) ||
			FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler))))
		{
			LOG_ERROR(LOG_TAG, "Failed to create DXC instances");
			return false;
		}

		DxcBuffer buffer = {};
		buffer.Ptr = aInput.source.data();
		buffer.Size = aInput.source.size();
		buffer.Encoding = DXC_CP_UTF8;

		std::vector<LPCWSTR> args;

		std::wstring entry = std::wstring(aInput.entryPoint.begin(), aInput.entryPoint.end());
		args.push_back(L"-E");
		args.push_back(entry.c_str());

		args.push_back(L"-T");
		args.push_back(StageToProfile(aInput.stage));

		if (aInput.debug)
		{
			args.push_back(L"-Zi");
			args.push_back(L"-Qembed_debug");
		}

		if (aInput.optimize)
		{
			args.push_back(L"-O3");
		}

		ComPtr<IDxcResult> result;

		HRESULT hr = compiler->Compile(
			&buffer,
			args.data(),
			(UINT32)args.size(),
			nullptr,
			IID_PPV_ARGS(&result)
		);

		if (FAILED(hr))
		{
			LOG_ERROR(LOG_TAG, "Compile call failed");
			return false;
		}

		ComPtr<IDxcBlobUtf8> errors;
		hr = result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
		if (SUCCEEDED(hr) && errors && errors->GetStringLength() > 0)
		{
			LOG_WARN(LOG_TAG, "{}", errors->GetStringPointer());
		}

		HRESULT status;
		if (FAILED(result->GetStatus(&status)) || FAILED(status))
		{
			LOG_ERROR(LOG_TAG, "Shader compilation failed");
			return false;
		}

		ComPtr<IDxcBlob> shaderBlob;
		hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);

		if (FAILED(hr) || !shaderBlob)
		{
			LOG_ERROR(LOG_TAG, "Failed to retrieve shader blob");
			return false;
		}

		LOG_INFO(LOG_TAG, "Shader compilation succeeded ({} bytes)", shaderBlob->GetBufferSize());

		outOutput.bytecode.resize(shaderBlob->GetBufferSize());
		memcpy(outOutput.bytecode.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());

		return true;
	}
}
