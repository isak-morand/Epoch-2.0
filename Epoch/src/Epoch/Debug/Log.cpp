#include "epch.h"
#include "Log.h"
#include <filesystem>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>

#pragma warning(push, 0)
#include <yaml-cpp/yaml.h>
#pragma warning(pop)

#include "Epoch/Core/Paths.h"

namespace Epoch
{
	void Log::Init()
	{
		SetDefaultTagSettings();
		LoadTagSettings();

		auto logPath = Epoch::Paths::GetLogDir() / "Epoch.log";
		std::vector<spdlog::sink_ptr> epochSinks{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/Epoch.log", true),
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()};

		epochSinks[0]->set_pattern("[%T] %l: %v%");
		epochSinks[1]->set_pattern("%^[%T] %v%$");

		spdlog::init_thread_pool(8192, 1);
		s_AsyncLogger = std::make_shared<spdlog::async_logger>("EPOCH_ASYNC", epochSinks.begin(), epochSinks.end(), spdlog::thread_pool());

		s_AsyncLogger->enable_backtrace(32);
		s_AsyncLogger->set_level(spdlog::level::trace);

		s_SyncLogger = std::make_shared<spdlog::logger>("EPOCH_SYNC", epochSinks.begin(), epochSinks.end());
		s_SyncLogger->set_level(spdlog::level::trace);
	}

	void Log::ShutDown()
	{
		spdlog::dump_backtrace();
		s_AsyncLogger.reset();
		s_SyncLogger.reset();
		spdlog::drop_all();

		SaveTagSettings();
	}

	Log::TagDetails& Log::GetTagDetails(LogTag aTag)
	{
		auto [it, inserted] = s_TagDetails.try_emplace(aTag.name, TagDetails{});
		return it->second;
	}

	void Log::SetDefaultTagSettings()
	{
		GetTagDetails(LogTags::Core);
		GetTagDetails(LogTags::Renderer);
		GetTagDetails(LogTags::Asset);
		GetTagDetails(LogTags::Physics);
		GetTagDetails(LogTags::Audio);
	}

	namespace
	{
		const char* LevelToString(Log::Level aLevel)
		{
			switch (aLevel)
			{
			case Log::Level::Trace: return "Trace";
			case Log::Level::Info: return "Info";
			case Log::Level::Warn: return "Warn";
			case Log::Level::Error: return "Error";
			case Log::Level::Fatal: return "Fatal";
			}
			return "Trace";
		}

		Log::Level LevelFromString(const std::string& aLevel)
		{
			if (aLevel == "Trace") return Log::Level::Trace;
			if (aLevel == "Info") return Log::Level::Info;
			if (aLevel == "Warn") return Log::Level::Warn;
			if (aLevel == "Error") return Log::Level::Error;
			if (aLevel == "Fatal") return Log::Level::Fatal;

			return Log::Level::Trace; // fallback
		}
	}

	void Log::LoadTagSettings()
	{
		const auto path = Epoch::Paths::GetConfigDir() / "Logging.yaml";

		if (!std::filesystem::exists(path))
		{
			return;
		}

		YAML::Node data = YAML::LoadFile(path.string());
		if (!data["Tags"])
		{
			return;
		}

		auto tags = data["Tags"];

		for (auto it = tags.begin(); it != tags.end(); ++it)
		{
			const std::string tagName = it->first.as<std::string>();
			const YAML::Node tagNode = it->second;

			TagDetails details;

			if (tagNode["Enabled"])
			{
				details.enabled = tagNode["Enabled"].as<bool>();
			}

			if (tagNode["Level"])
			{
				details.levelFilter = LevelFromString(tagNode["Level"].as<std::string>());
			}

			s_TagDetails[tagName] = details;
		}
	}

	void Log::SaveTagSettings()
	{
		const auto path = Epoch::Paths::GetConfigDir() / "Logging.yaml";

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Tags" << YAML::Value;
		out << YAML::BeginMap;

		for (const auto& [tagName, details] : s_TagDetails)
		{
			out << YAML::Key << tagName;
			out << YAML::Value;

			out << YAML::BeginMap;
			out << YAML::Key << "Enabled" << YAML::Value << details.enabled;
			out << YAML::Key << "Level" << YAML::Value << LevelToString(details.levelFilter);
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
	}
}
