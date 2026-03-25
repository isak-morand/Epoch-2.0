#include <iostream>
#include <Epoch/Core/Engine.h>
#include "App.h"

#if PLATFORM_WINDOWS && _DEBUG
#include <windows.h>
#include <stdio.h>

void CreateConsole()
{
    AllocConsole();
    freopen("CONIN$", "rb", stdin);
    freopen("CONOUT$", "wb", stdout);
    freopen("CONOUT$", "wb", stderr);
}
#endif

int main(int argc, char** argv)
{
#if PLATFORM_WINDOWS && _DEBUG
    CreateConsole();
#endif

    Epoch::EngineProperties props;
    Epoch::Engine engine(props);
    App app;
    engine.Run(&app);

#if PLATFORM_WINDOWS && _DEBUG
    FreeConsole();
#endif
}
