#include "base/prerequisites.h"
#include "core/core.h"
#include "../main_module/main_module.h"
#include "../main_memory/main_memory.h"
using namespace Arieo;
#include <stdio.h>
#include <fstream>

static Arieo::MainModule g_main_module;
ARIEO_DLLEXPORT int MainEntry(void* app, const char* manifest_context)
{
    Base::Memory::MemoryManager::initialize(
        g_main_module.getMainMemoryManager()
    );

    Arieo::Base::Interop::SharedRef<Interface::Main::IMainModule> main_module_interface = Base::Interop::makePersistentShared<Interface::Main::IMainModule>(g_main_module);

    Core::Logger::setDefaultLogger("main");
    
    Core::Logger::info("Main module initializing.");
    g_main_module.init(app);
    
    Core::ModuleManager::registerInterface<Interface::Main::IMainModule>(
        "main_module",
        main_module_interface
    );

    // Load manifest context from file path specified by environment variable
    {
        std::string app_manifest_path = Arieo::Core::SystemUtility::Environment::getEnvironmentValue("APP_MANIFEST_PATH");
        std::ifstream manifest_file(app_manifest_path);
        if (!manifest_file.is_open()) {
            Core::Logger::error("Failed to open manifest file at path: {}", app_manifest_path);
            return -1;
        }

        g_main_module.loadManifest(std::string_view(manifest_context));
    }

    while(true)
    {
        g_main_module.tick();
    }
    
    g_main_module.deinit();
}




