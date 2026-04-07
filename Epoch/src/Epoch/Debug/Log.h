#pragma once
#include <spdlog/spdlog.h>
#include "Assert.h"

struct LogTag
{
	const char* name;
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
		inline static std::shared_ptr<spdlog::logger> s_AsyncLogger;
		inline static std::shared_ptr<spdlog::logger> s_SyncLogger;
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
		EPOCH_ASSERT(s_AsyncLogger, "Logger used before initialization!");

		switch (aLevel)
		{
		case Level::Debug: s_AsyncLogger->trace("[{}] {}", aTag.name, std::format(aFormat, std::forward<Args>(aArgs)...)); break;
		case Level::Info:  s_AsyncLogger->info("[{}] {}", aTag.name, std::format(aFormat, std::forward<Args>(aArgs)...)); break;
		case Level::Warn:  s_AsyncLogger->warn("[{}] {}", aTag.name, std::format(aFormat, std::forward<Args>(aArgs)...)); break;
		case Level::Error: s_AsyncLogger->error("[{}] {}", aTag.name, std::format(aFormat, std::forward<Args>(aArgs)...)); break;
		case Level::Fatal: s_AsyncLogger->critical("[{}] {}", aTag.name, std::format(aFormat, std::forward<Args>(aArgs)...)); break;
		}
	}

	template<typename ...Args>
	inline void Log::PrintAssertMessage(const char* aCondition, const char* aFile, int aLine, const char* aFunction, std::format_string<Args...> aMessage, Args&&... aArgs)
	{
		const std::string userMessage = std::format(aMessage, std::forward<Args>(aArgs)...);

		const std::string finalMessage = std::format(
			"Assertion Failed!\n\nCondition: {}\nFile: {}\nLine: {}\nFunction: {}\n\n{}",
			aCondition, aFile, aLine, aFunction, userMessage);

		s_SyncLogger->critical("{}", finalMessage);
		s_SyncLogger->flush();

#ifdef EPOCH_WIN32
		MessageBoxA(nullptr, finalMessage.c_str(), "Epoch Assert", MB_OK | MB_ICONERROR);
#endif
	}
}
