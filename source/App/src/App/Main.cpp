#include <Epoch/Core/EntryPoint.h>
#include <Epoch/Core/Engine.h>
#include "App.h"

void Main(Epoch::Args aArgs)
{
	Epoch::EngineDesc desc;
	desc.window.startMaximized = true;
	desc.renderer.graphicsAPI = Epoch::RHI::APIFromString(aArgs.GetString("API", "d3d12"));

#ifdef _DEBUG
	desc.renderer.enableDebugRuntime = true;
	desc.renderer.enableGPUValidation = true;
	desc.renderer.enableWarningsAsErrors = true;
#endif
	desc.renderer.enableDebugRuntime = aArgs.GetBool("debug-runtime", desc.renderer.enableDebugRuntime);
	desc.renderer.enableGPUValidation = aArgs.GetBool("gpu-validation", desc.renderer.enableGPUValidation);
	desc.renderer.enableWarningsAsErrors = aArgs.GetBool("warnings-as-errors", desc.renderer.enableWarningsAsErrors);

	Epoch::Engine engine(desc);
	App app;
	engine.SetApp(&app);
	engine.Run();
}
