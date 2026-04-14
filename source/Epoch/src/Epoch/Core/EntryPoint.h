#include <Epoch/Core/Paths.h>
#include <Epoch/Debug/Log.h>

extern void Main(int aArgc, char** aArgv);

#ifdef EPOCH_PLATFORM_WINDOWS
static void AddDllSearchPath()
{
	SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);

	std::filesystem::path exePath(buffer);
	auto libPath = exePath.parent_path() / L"lib";

	AddDllDirectory(libPath.c_str());
}
#endif

#if defined(EPOCH_PLATFORM_WINDOWS) && defined(_RELEASE)  
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main()
#endif
{
#ifdef EPOCH_PLATFORM_WINDOWS
	AddDllSearchPath();
#endif

	Epoch::Paths::Init();
	Epoch::Log::Init();

	/*if (__argc > 1)
	{
		std::vector<std::string> args(__argc - 1);
		for (size_t i = 1; i < __argc; i++)
		{
			args[i - 1] = std::string(__argv[i]);
		}

		for (auto& arg : args)
		{
			LOG_DEBUG(LogTags::Core, "{}", arg);
		}
	}*/

	Main(__argc, __argv);

	Epoch::Log::ShutDown();
}
