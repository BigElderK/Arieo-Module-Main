#pragma once

#include <filesystem>
#include <regex>
#include <string>
#include "core/singleton/singleton.h"
#include "core/thread/thread_pool.h"
#include "core/coroutine/coroutine.h"
#include "core/job/job_system.h"
#include "core/manifest/manifest.h"
#include "base/interop/interface.h"

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

        void loadManifest(const Base::InteropOld<std::string_view>& manifest_context) override;
        void enqueueTask(Arieo::Core::Coroutine::Task::Tasklet&& task) override;

        void registerTickable(Base::InteropOld<Interface::Main::ITickable>) override;
        void unregisterTickable(Base::InteropOld<Interface::Main::ITickable>) override;

        Base::Memory::MemoryManager* getMainMemoryManager() override;
        Base::Interop::SharedRef<Interface::Archive::IArchive> getRootArchive() override;

        void* getAppHandle() override;

        Base::InteropOld<std::string_view> getManifestContext() override;

        void init(void* app_handle);
        void tick();
        void deinit();
    protected:
        std::vector<Base::InteropOld<Interface::Main::ITickable>> m_register_tickable_array;

        Core::ThreadPool m_thread_pool;
        Core::JobSystem m_job_system;

        Base::Interop::SharedRef<Interface::Archive::IArchive> m_root_archive = nullptr;
        void* m_app_handle = nullptr;

        Core::Manifest m_manifest;
        std::string m_manifest_context;

        Base::InteropOld<Interface::Script::IInstance> m_startup_script_instance = nullptr;
    };
}
