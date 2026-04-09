#pragma once
#include <filesystem>

namespace Epoch::Paths
{
	void Init();

	const std::filesystem::path& GetConfigDir();
	const std::filesystem::path& GetLogDir();
}
