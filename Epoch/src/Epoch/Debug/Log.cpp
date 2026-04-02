#include "epch.h"
#include "Log.h"
#include <filesystem>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>

namespace Epoch
{
	void Log::Init()
	{
		std::string logsDirectory = "logs";
		if (!std::filesystem::exists(logsDirectory))
		{
			std::filesystem::create_directories(logsDirectory);
		}

		std::vector<spdlog::sink_ptr> epochSinks =
		{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/Epoch.log", true),
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
		};

		epochSinks[0]->set_pattern("[%T] %l: %v%");
		epochSinks[1]->set_pattern("%^[%T] %v%$");

		spdlog::init_thread_pool(8192, 1);
		staticAsyncLogger = std::make_shared<spdlog::async_logger>("EPOCH", epochSinks.begin(), epochSinks.end(), spdlog::thread_pool());

		staticAsyncLogger->enable_backtrace(32);
		staticAsyncLogger->set_level(spdlog::level::trace);

		staticSyncLogger = std::make_shared<spdlog::logger>("EPOCH", epochSinks.begin(), epochSinks.end());
	}

	void Log::ShutDown()
	{
		spdlog::dump_backtrace();
		staticAsyncLogger.reset();
		spdlog::drop_all();
	}
}
