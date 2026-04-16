#include "epch.h"
#include "NVRHIMessageCallback.h"

namespace Epoch
{
#define LOG_TAG LogTags::Renderer

	NVRHIMessageCallback& NVRHIMessageCallback::GetInstance()
	{
		static NVRHIMessageCallback instance;
		return instance;
	}

	void NVRHIMessageCallback::message(nvrhi::MessageSeverity aSeverity, const char* aMessageText)
	{
		switch (aSeverity)
		{
		case nvrhi::MessageSeverity::Info:
			LOG_INFO(LOG_TAG, "{}", aMessageText);
			break;
		case nvrhi::MessageSeverity::Warning:
			LOG_WARN(LOG_TAG, "{}", aMessageText);
			break;
		case nvrhi::MessageSeverity::Error:
			LOG_ERROR(LOG_TAG, "{}", aMessageText);
			break;
		case nvrhi::MessageSeverity::Fatal:
			LOG_FATAL(LOG_TAG, "{}", aMessageText);
			break;
		}
	}
}