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
    void MainModule::loadManifest(const std::string& manifest_file_path)
    {
        // Sperator manifest_file_path to parent folder and file name.
        std::filesystem::path manifest_path(manifest_file_path);
        std::string manifest_folder = manifest_path.parent_path().string();
        std::string manifest_file = manifest_path.filename().string();

        Interface::Archive::IArchiveManager* archive_manager = nullptr;
        {
            if (manifest_folder.ends_with(".obb") || manifest_folder.ends_with(".zip"))
            {
                // if manifest_folder contains .obb or .zip, create obb archive to load manifest file.
                Core::ModuleManager::getProcessSingleton().loadModuleLib(
                    Base::StringUtility::format(
                        "{}/{}",
                        Core::SystemUtility::FileSystem::getFormalizedPath("${MODULE_DIR}"), 
                        Core::SystemUtility::Lib::getDymLibFileName("arieo_obb_archive_module")
                    ),
                    getMainMemoryManager()
                );
                archive_manager = Core::ModuleManager::getInterface<Interface::Archive::IArchiveManager>("obb_archive");
            }
            else
            {
                // create os filesystem archive to load manifest file.
                Core::ModuleManager::getProcessSingleton().loadModuleLib(
                    Base::StringUtility::format(
                        "{}/{}",
                        Core::SystemUtility::FileSystem::getFormalizedPath("${MODULE_DIR}"), 
                        Core::SystemUtility::Lib::getDymLibFileName("arieo_os_filesystem_archive_module")
                    ),
                    getMainMemoryManager()
                );
                archive_manager = Core::ModuleManager::getInterface<Interface::Archive::IArchiveManager>("os_filesystem_archive");
            }

            if (archive_manager == nullptr)
            {
                Core::Logger::error("Cannot found module to load obb archive: {}", manifest_folder);
                return;
            }
        }

        // Create root archive to load manifest file.
        {
            m_root_archive = archive_manager->createArchive(manifest_folder);
            if (m_root_archive == nullptr)
            {
                Core::Logger::error("Create archive failed: {}", manifest_folder);
                return;
            }
            Core::Logger::info("Create archive success: {}", manifest_folder);
        }

        // Load manifest file from obb archive.
        {
            auto [manifest_content_buffer, manifest_content_size] = m_root_archive->getFileBuffer(manifest_file);
            if (manifest_content_buffer == nullptr)
            {
                Core::Logger::error("Cannot load manifest file in obb archive: {}", manifest_file);
                return;
            }

            Core::Logger::trace("Processing manifest {}", manifest_file_path);
            m_manifest_content = std::string(static_cast<const char*>(manifest_content_buffer), manifest_content_size);
            loadManifestContent(manifest_file_path);
        }
    }

    void MainModule::loadManifestContent(const std::string& manifest_file_path)
    {
        Core::Logger::info("Parsing manifest file: {}", manifest_file_path);
        Core::ConfigNode&& config_node = Core::ConfigFile::Load(m_manifest_content);
        if(config_node.IsNull())
        {
            Core::Logger::error("Load manifest file failed: {}", manifest_file_path);
            return;
        }

        Core::Logger::info("Parsing app info");
        if(config_node["app"].IsDefined() == false)
        {
            Core::Logger::error("Cannot found 'app' node in: {}", manifest_file_path);
            return;
        }

        Core::Logger::info("Parsing app.host_os info");
        if(config_node["app"]["host_os"].IsDefined() == false)
        {
            Core::Logger::error("Cannot found 'app.host_os' node in: {}", manifest_file_path);
            return;
        }

        Core::Logger::info("Parsing app.host_os.{} info", Core::SystemUtility::getHostOSName());
        Core::ConfigNode system_node = config_node["app"]["host_os"][Core::SystemUtility::getHostOSName()];
        if(system_node.IsDefined() == false)
        {
            Core::Logger::error("Cannot found 'app.host_os.{}' node in: {}", Core::SystemUtility::getHostOSName(), manifest_file_path);
            return;
        }

        Core::Logger::info("Parsing app.host_os.{}.environment info", Core::SystemUtility::getHostOSName());
        //Iterator module->environments node to set coresponding environment, before load any libs.
        for (Core::ConfigNode::const_iterator env_node_iter = system_node["environments"].begin();
            env_node_iter != system_node["environments"].end();
            ++env_node_iter) 
        {
            std::string&& env_name = env_node_iter->first.as<std::string>();

            if(env_node_iter->second.IsSequence())
            {
                //If the value is in the seq mode, we append them to current environment.
                for (Core::ConfigNode::const_iterator env_value_iter = env_node_iter->second.begin();
                    env_value_iter != env_node_iter->second.end();
                    ++env_value_iter) 
                {
                    std::string&& append_env_value = env_value_iter->as<std::string>();
                    Core::Logger::info("Prepend Environment: {} = {}", env_name, append_env_value);
                    Core::SystemUtility::Environment::prependEnvironmentValue(
                        env_name, 
                        Core::SystemUtility::FileSystem::getFormalizedPath(append_env_value));
                }
            }
            else
            {
                //Replace the current environment
                std::string&& env_value = env_node_iter->second.as<std::string>();
                Core::Logger::info("Set Environment: {} = {}", env_name, env_value);
                Core::SystemUtility::Environment::setEnvironmentValue(
                    env_name, 
                    Core::SystemUtility::FileSystem::getFormalizedPath(env_value));
            }
        }

        Core::Logger::info("Parsing app.host_os.{}.modules info", Core::SystemUtility::getHostOSName());
        Core::ConfigNode&& module_nodes = system_node["modules"];
        if(module_nodes.IsNull() == false)
        {
            Core::Logger::info("Before loading Module DymLib");
            for (Core::ConfigNode::const_iterator module_node_iter = module_nodes.begin();
                module_node_iter != module_nodes.end();
                ++module_node_iter) 
            {
                Core::Logger::info("Prepare loading Module DymLib: {}", module_node_iter->as<std::string>());
                std::string&& lib_file_path = Core::SystemUtility::FileSystem::getFormalizedPath(module_node_iter->as<std::string>());                    
                Core::Logger::info("Loading Module DymLib: {}", lib_file_path.c_str());
                Core::ModuleManager::getProcessSingleton().loadModuleLib(lib_file_path, getMainMemoryManager());
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

    const std::string& MainModule::getManifestContent()
    {
        return m_manifest_content;
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
}