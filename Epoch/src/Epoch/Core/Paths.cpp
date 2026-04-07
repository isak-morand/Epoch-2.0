#include "epch.h"
#include "Paths.h"

namespace Epoch::Paths
{
    static std::filesystem::path s_ConfigDir = "Config";
    static std::filesystem::path s_LogDir = "Logs";

    void Init()
    {
        std::filesystem::create_directories(s_ConfigDir);
        std::filesystem::create_directories(s_LogDir);
    }

    const std::filesystem::path& GetConfigDir() { return s_ConfigDir; }
    const std::filesystem::path& GetLogDir() { return s_LogDir; }
}
