#include <Epoch/Core/EntryPoint.h>
#include "App.h"
#include <Epoch/Core/Engine.h>

void Main(int aArgc, char** aArgv)
{
    Epoch::EngineDesc desc;
    desc.window.title = "Test app";
    desc.renderer.graphicsAPI = Epoch::GraphicsAPI::D3D12;

    Epoch::Engine engine(desc);
    App app;
    engine.SetApp(&app);
    engine.Run();
}
