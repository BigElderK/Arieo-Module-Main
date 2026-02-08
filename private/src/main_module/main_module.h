#pragma once

#include <filesystem>
#include <regex>
#include <string>
#include "core/singleton/singleton.h"
#include "core/thread/thread_pool.h"
#include "core/coroutine/coroutine.h"
#include "core/job/job_system.h"
#include "interface/main/main_module.h"
#include "interface/script/script.h"
namespace Arieo
{
    class MainModule
        : public Interface::Main::IMainModule
    {        
    public:
        MainModule();
        ~MainModule();

        void loadManifest(const std::string& manifest_path) override;
        void enqueueTask(Arieo::Core::Coroutine::Task::Tasklet&& task) override;

        void registerTickable(Interface::Main::ITickable*) override;
        void unregisterTickable(Interface::Main::ITickable*) override;

        Base::Memory::MemoryManager* getMainMemoryManager() override;
        Interface::Archive::IArchive* getRootArchive() override;

        void* getAppHandle() override;
        const std::string& getManifestContent() override;

        void init(void* app_handle);
        void tick();
        void deinit();
    protected:
        std::vector<Interface::Main::ITickable*> m_register_tickable_array;

        Core::ThreadPool m_thread_pool;
        Core::JobSystem m_job_system;

        void loadManifestContent(const std::string& manifest_file_path);

        Interface::Archive::IArchive* m_root_archive = nullptr;
        void* m_app_handle = nullptr;

        std::string m_manifest_content;

        Interface::Script::IInstance* m_startup_script_instance = nullptr;
    };
}
