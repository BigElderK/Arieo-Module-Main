cmake_minimum_required (VERSION 3.31)

include($ENV{ARIEO_BUILDENV_PACKAGE_INSTALL_FOLDER}/cmake/build_environment.cmake)
project(ArieoEngine)

if(ARIEO_HOST_OS STREQUAL "android")
    # Check if game-activity is available before building
    find_package(game-activity QUIET)
    if(TARGET game-activity::game-activity_static)
        arieo_engine_project(
        arieo_main_module
        PROJECT_TYPE module

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
            Arieo-Core::arieo_core
            mimalloc::mimalloc
            game-activity::game-activity_static
            log
            android

        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*/*.cpp
    )
    set_target_properties(arieo_main_module PROPERTIES LINK_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")
    else()
        # game-activity not available on Android platform
        message(FATAL_ERROR "Arieo-Module-Main: Android build failed - game-activity package not found. Check CMAKE_PREFIX_PATH includes the gradle install folder.")
    endif()
else()
    # Non-Android build
    arieo_engine_project(
        arieo_main_module
        PROJECT_TYPE module

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
            Arieo-Core::arieo_core
            mimalloc::mimalloc

        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*/*.cpp
    )
endif()