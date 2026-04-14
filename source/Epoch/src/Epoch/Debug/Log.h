#pragma once
#include <unordered_map>
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
		enum class Level : uint8_t { Trace, Info, Warn, Error, Fatal };
		
		struct TagDetails
		{
			bool enabled = true;
			Level levelFilter = Level::Info;
		};

	public:
		static void Init();
		static void ShutDown();

		template<typename... Args>
		static void PrintMessageTag(Log::Level aLevel, LogTag aTag, std::format_string<Args...> aFormat, Args&&... aArgs);

		template<typename... Args>
		static void PrintAssertMessage(const char* aCondition, const char* aFile, int aLine, const char* aFunction, std::format_string<Args...> aMessage, Args&&... aArgs);

	private:
		static TagDetails& GetTagDetails(LogTag aTag);

		static void SetDefaultTagSettings();
		static void LoadTagSettings();
		static void SaveTagSettings();

	private:
		inline static std::shared_ptr<spdlog::logger> s_AsyncLogger;
		inline static std::shared_ptr<spdlog::logger> s_SyncLogger;

		inline static std::unordered_map<std::string, TagDetails> s_TagDetails;
	};
}

#define LOG_TRACE(LogTag, ...)		::Epoch::Log::PrintMessageTag(::Epoch::Log::Level::Trace, LogTag, __VA_ARGS__)
#define LOG_INFO(LogTag, ...)		::Epoch::Log::PrintMessageTag(::Epoch::Log::Level::Info, LogTag, __VA_ARGS__)
#define LOG_WARN(LogTag, ...)	::Epoch::Log::PrintMessageTag(::Epoch::Log::Level::Warn, LogTag, __VA_ARGS__)
#define LOG_ERROR(LogTag, ...)		::Epoch::Log::PrintMessageTag(::Epoch::Log::Level::Error, LogTag, __VA_ARGS__)
#define LOG_FATAL(LogTag, ...)		::Epoch::Log::PrintMessageTag(::Epoch::Log::Level::Fatal, LogTag, __VA_ARGS__)

namespace Epoch
{
	template<typename... Args>
	void Log::PrintMessageTag(Level aLevel, LogTag aTag, std::format_string<Args...> aFormat, Args&&... aArgs)
	{
		EPOCH_ASSERT(s_AsyncLogger, "Logger used before initialization!");

		auto& tagDetails = GetTagDetails(aTag);

		if (!tagDetails.enabled || aLevel < tagDetails.levelFilter)
		{
			return;
		}

		const std::string message = std::format(aFormat, std::forward<Args>(aArgs)...);
		switch (aLevel)
		{
		case Level::Trace: s_AsyncLogger->trace("[{}] {}", aTag.name, message); break;
		case Level::Info:  s_AsyncLogger->info("[{}] {}", aTag.name, message); break;
		case Level::Warn:  s_AsyncLogger->warn("[{}] {}", aTag.name, message); break;
		case Level::Error: s_AsyncLogger->error("[{}] {}", aTag.name, message); break;
		case Level::Fatal: s_AsyncLogger->critical("[{}] {}", aTag.name, message); break;
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

#ifdef EPOCH_PLATFORM_WINDOWS
		MessageBoxA(nullptr, finalMessage.c_str(), "Epoch Assert", MB_OK | MB_ICONERROR);
#endif
	}
}
