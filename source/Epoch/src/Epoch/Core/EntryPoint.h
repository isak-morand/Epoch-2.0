#include "Args.h"
#include "Paths.h"
#include "Epoch/Debug/Log.h"

extern void Main(Epoch::Args aArgs);

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

	Epoch::Args args(__argc, __argv);

	Epoch::Paths::Init();
	Epoch::Log::Init();

	Main(args);

	Epoch::Log::ShutDown();
}
