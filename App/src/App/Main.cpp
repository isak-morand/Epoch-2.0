#include <Epoch/Core/EntryPoint.h>
#include <Epoch/Core/Engine.h>
#include "App.h"

void Main(int aArgc, char** aArgv)
{
    Epoch::EngineDesc desc;
    desc.window.title = "Test app";
    //desc.window.startBorderless = true;
    //desc.window.startFullscreen = true;
    //desc.window.startMaximized = true;
    desc.renderer.graphicsAPI = Epoch::GraphicsAPI::D3D12;

    Epoch::Engine engine(desc);
    App app;
    engine.SetApp(&app);
    engine.Run();
}
