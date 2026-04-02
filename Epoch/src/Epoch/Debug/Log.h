#pragma once
#include <spdlog/spdlog.h>
#include "Assert.h"

struct LogTag
{
	const char* Name;
};

namespace LogTags
{
	inline constexpr LogTag Core{ "Core" };
	inline constexpr LogTag Renderer{ "Renderer" };
	inline constexpr LogTag Asset{ "Asset" };
	inline constexpr LogTag Physics{ "Physics" };
	inline constexpr LogTag Audio{ "Audio" };
}

namespace Epoch
{
	class Log
	{
	public:
		enum class Level : uint8_t { Debug, Info, Warn, Error, Fatal };

	public:
		static void Init();
		static void ShutDown();

		template<typename... Args>
		static void PrintMessageTag(Log::Level aLevel, LogTag aTag, std::format_string<Args...> aFormat, Args&&... aArgs);

		template<typename... Args>
		static void PrintAssertMessage(const char* aCondition, const char* aFile, int aLine, const char* aFunction, std::format_string<Args...> aMessage, Args&&... aArgs);

	private:
		inline static std::shared_ptr<spdlog::logger> staticAsyncLogger;
		inline static std::shared_ptr<spdlog::logger> staticSyncLogger;
	};
}

#define LOG_DEBUG(LogTag, ...)		::Epoch::Log::PrintMessageTag(::Epoch::Log::Level::Debug, LogTag, __VA_ARGS__)
#define LOG_INFO(LogTag, ...)		::Epoch::Log::PrintMessageTag(::Epoch::Log::Level::Info, LogTag, __VA_ARGS__)
#define LOG_WARNING(LogTag, ...)	::Epoch::Log::PrintMessageTag(::Epoch::Log::Level::Warn, LogTag, __VA_ARGS__)
#define LOG_ERROR(LogTag, ...)		::Epoch::Log::PrintMessageTag(::Epoch::Log::Level::Error, LogTag, __VA_ARGS__)
#define LOG_FATAL(LogTag, ...)		::Epoch::Log::PrintMessageTag(::Epoch::Log::Level::Fatal, LogTag, __VA_ARGS__)

namespace Epoch
{
	template<typename... Args>
	void Log::PrintMessageTag(Log::Level aLevel, LogTag aTag, std::format_string<Args...> aFormat, Args&&... aArgs)
	{
		EPOCH_ASSERT(staticAsyncLogger, "Logger used before initialization!");

		switch (aLevel)
		{
		case Level::Debug: staticAsyncLogger->trace("[{}] {}", aTag.Name, std::format(aFormat, std::forward<Args>(aArgs)...)); break;
		case Level::Info:  staticAsyncLogger->info("[{}] {}", aTag.Name, std::format(aFormat, std::forward<Args>(aArgs)...)); break;
		case Level::Warn:  staticAsyncLogger->warn("[{}] {}", aTag.Name, std::format(aFormat, std::forward<Args>(aArgs)...)); break;
		case Level::Error: staticAsyncLogger->error("[{}] {}", aTag.Name, std::format(aFormat, std::forward<Args>(aArgs)...)); break;
		case Level::Fatal: staticAsyncLogger->critical("[{}] {}", aTag.Name, std::format(aFormat, std::forward<Args>(aArgs)...)); break;
		}
	}

	template<typename ...Args>
	inline void Log::PrintAssertMessage(const char* aCondition, const char* aFile, int aLine, const char* aFunction, std::format_string<Args...> aMessage, Args&&... aArgs)
	{
		const std::string userMessage = std::format(aMessage, std::forward<Args>(aArgs)...);

		const std::string finalMessage = std::format(
			"Assertion Failed!\n\nCondition: {}\nFile: {}\nLine: {}\nFunction: {}\n\n{}",
			aCondition, aFile, aLine, aFunction, userMessage);

		staticSyncLogger->critical("{}", finalMessage);
		staticSyncLogger->flush();

#ifdef EPOCH_WIN32
		MessageBoxA(nullptr, finalMessage.c_str(), "Epoch Assert", MB_OK | MB_ICONERROR);
#endif
	}
}
