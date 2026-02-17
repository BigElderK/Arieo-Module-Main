#include "base/prerequisites.h"
#include "core/core.h"
#include "../main_module/main_module.h"
#include "../main_memory/main_memory.h"
using namespace Arieo;
#include <stdio.h>
#include <fstream>

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

    // Load manifest context from file path specified by environment variable
    {
        std::string app_manifest_path = Arieo::Core::SystemUtility::Environment::getEnvironmentValue("APP_MANIFEST_PATH");
        std::ifstream manifest_file(app_manifest_path);
        if (!manifest_file.is_open()) {
            Core::Logger::error("Failed to open manifest file at path: {}", app_manifest_path);
            return -1;
        }

        std::string manifest_content((std::istreambuf_iterator<char>(manifest_file)),
                                     std::istreambuf_iterator<char>());
        manifest_file.close();

        g_main_module.loadManifest(manifest_content);
    }

    while(true)
    {
        g_main_module.tick();
    }
    
    g_main_module.deinit();
}