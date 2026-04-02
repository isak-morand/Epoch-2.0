#include <Epoch/Debug/Log.h>

void Main(int aArgc, char** aArgv);

extern void Main(int aArgc, char** aArgv);

#if defined(EPOCH_WIN32) && defined(_DIST)  
#include <windows.h>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, const char** argv)
#endif
{
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
