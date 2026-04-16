#pragma once
#include <nvrhi/nvrhi.h>

namespace Epoch
{
	struct NVRHIMessageCallback : public nvrhi::IMessageCallback
	{
		static NVRHIMessageCallback& GetInstance();

		void message(nvrhi::MessageSeverity aSeverity, const char* aMessageText) override;
	};
}
