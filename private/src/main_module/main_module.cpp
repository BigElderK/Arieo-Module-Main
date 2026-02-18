#include "base/prerequisites.h"
#include "core/core.h"
#include "core/config/config.h"

#include "main_module.h"
#include "../main_memory/main_memory.h"

using namespace Arieo;
#include "interface/archive/archive.h"
#include "base/prerequisites.h"
#include "core/core.h"

namespace Arieo
{
    void MainModule::loadManifest(std::string manifest_context)
    {
        m_manifest_context = manifest_context;
        m_manifest.loadFromString(manifest_context);

        m_manifest.applyPresetEnvironments();

        // Load all modules
        auto module_paths = m_manifest.getAllEngineModulePaths();
        for (const auto& module_path : module_paths)        
        {
            Core::Logger::info("Loading Module DymLib: {}", module_path.string());
            if(module_path.string().find("main_module") != std::string::npos)
            {
                continue;
            }
            Core::ModuleManager::getProcessSingleton().loadModuleLib(module_path.string(), getMainMemoryManager());
        }

        // Load root archive
        {
            Interface::Archive::IArchiveManager* archive_factory = Core::ModuleManager::getInterface<Interface::Archive::IArchiveManager>();
            if(archive_factory == nullptr)
            {
                Core::Logger::fatal("No archive factory module found!");
            }

            std::filesystem::path content_root_path = Core::SystemUtility::FileSystem::getFormalizedPath(
                m_manifest.getSystemNode()["environments"]["CONTENT_ROOT"].as<std::string>()
            );

            m_root_archive = archive_factory->createArchive(content_root_path);
            if(m_root_archive == nullptr)
            {
                Core::Logger::fatal("Failed to create root archive with path: {}", content_root_path.string());
            }
        }
    }

    void MainModule::enqueueTask(Arieo::Core::Coroutine::Task::Tasklet&& task)
    {
        m_job_system.enqueueTask(std::move(Core::Coroutine::Task(std::move(task))));
    }

    MainModule::MainModule()
    {
    }

    MainModule::~MainModule()
    {

    }

    Interface::Archive::IArchive* MainModule::getRootArchive()
    {
        return m_root_archive;
    }

    void MainModule::registerTickable(Interface::Main::ITickable* tickable)
    {
        m_register_tickable_array.emplace_back(tickable);
        tickable->onInitialize();
    }

    void MainModule::unregisterTickable(Interface::Main::ITickable* tickable)
    {
        std::erase_if(m_register_tickable_array, [tickable](Interface::Main::ITickable* register_tickable)
        {
            if(register_tickable == tickable)
            {
                register_tickable->onDeinitialize();
                return true;
            }
            else
            {
                return false;
            }
        });
    }

    void MainModule::init(void* app_handle)
    {
        m_app_handle = app_handle;
        m_thread_pool.start();

        // std::for_each(
        //     m_register_tickable_array.begin(), 
        //     m_register_tickable_array.end(),
        //     [](Interface::Main::ITickable* register_tickable)
        //     {
        //         register_tickable->onInitialize();
        //     }
        // );
    }

    void MainModule::tick()
    {
        // std::future<void> job_finished = Arieo::Core::MainModuleContext::getProcessSingleton().m_job_system.updateOneFrame(
        //     Arieo::Core::MainModuleContext::getProcessSingleton().m_thread_pool,
        //     1
        // );
        // job_finished.wait();
        m_job_system.updateOneFrame();

        std::for_each(
            m_register_tickable_array.begin(), 
            m_register_tickable_array.end(),
            [](Interface::Main::ITickable* register_tickable)
            {
                register_tickable->onTick();
            }
        );

        std::this_thread::yield();
    }

    void MainModule::deinit()
    {
        std::for_each(
            m_register_tickable_array.begin(), 
            m_register_tickable_array.end(),
            [](Interface::Main::ITickable* register_tickable)
            {
                register_tickable->onDeinitialize();
            }
        );
        m_register_tickable_array.clear();
    }

    Base::Memory::MemoryManager* MainModule::getMainMemoryManager()
    {
        return MainMemory::getMainMemoryManager();
    }

    void* MainModule::getAppHandle()
    {
        return m_app_handle;
    }

    std::string MainModule::getManifestContext()
    {
        return m_manifest_context;
    }
}