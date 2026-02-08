#include "base/prerequisites.h"
#include "core/core.h"
#include "../main_module/main_module.h"
#include "../main_memory/main_memory.h"
using namespace Arieo;
#include <stdio.h>

static Arieo::MainModule g_main_module;
ARIEO_DLLEXPORT int MainEntry(void* app)
{
    Base::Memory::MemoryManager::initialize(g_main_module.getMainMemoryManager());
    Core::Logger::setDefaultLogger("main");
    
    Core::Logger::info("Main module initializing.");
    g_main_module.init(app);
    
    Core::ModuleManager::registerInterface<Interface::Main::IMainModule>(
        "main_module",
        &g_main_module
    );

    g_main_module.loadManifest(Arieo::Core::SystemUtility::Environment::getEnvironmentValue("APP_MANIFEST_PATH"));

    while(true)
    {
        g_main_module.tick();
    }
    
    g_main_module.deinit();
}