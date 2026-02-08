cmake_minimum_required (VERSION 3.31)

include($ENV{ARIEO_BUILDENV_PACKAGE_INSTALL_FOLDER}/cmake/build_environment.cmake)
project(ArieoEngine)

if(ARIEO_HOST_OS STREQUAL "android")
    arieo_engine_project(
        arieo_main_module
        ALIAS arieo::modules::main
        PROJECT_TYPE module
        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*/*.cpp
        PACKAGES
            Arieo-Interface-Main
            Arieo-Interface-Archive
            Arieo-Interface-Script
            mimalloc
            game-activity
            Arieo-Core
        INTERFACES
            Arieo-Interface-Main::arieo_main_interface
            Arieo-Interface-Archive::arieo_archive_interface
            Arieo-Interface-Script::arieo_script_interface
        PRIVATE_LIBS
            arieo::base
            arieo::core
            mimalloc::mimalloc
            game-activity::game-activity_static
            log
            android
    )
    set_target_properties(arieo_main_module PROPERTIES LINK_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")
else()
    arieo_engine_project(
        arieo_main_module
        ALIAS arieo::modules::main
        PROJECT_TYPE module
        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*/*.cpp
        PACKAGES
            Arieo-Interface-Main
            Arieo-Interface-Archive
            Arieo-Interface-Script
            mimalloc
            Arieo-Core
        INTERFACES
            Arieo-Interface-Main::arieo_main_interface
            Arieo-Interface-Archive::arieo_archive_interface
            Arieo-Interface-Script::arieo_script_interface
        PRIVATE_LIBS
            arieo::base
            arieo::core
            mimalloc::mimalloc
    )
endif()