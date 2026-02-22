cmake_minimum_required (VERSION 3.31)

include($ENV{ARIEO_BUILD_ENV_PACKAGES_OUTPUT_DIR}/cmake/build_environment.cmake)

ARIEO_PACKAGE(
    Arieo-Module-Main
    CATEGORY 02_engine
    DEPENDS
        https://github.com/BigElderK/Arieo-Core.git@main
        https://github.com/BigElderK/Arieo-Interface-Main.git@main
        https://github.com/BigElderK/Arieo-Interface-Archive.git@main
        https://github.com/BigElderK/Arieo-Interface-Script.git@main
)

if(ARIEO_HOST_OS STREQUAL "android")
    # Check if game-activity is available before building
    find_package(game-activity QUIET)
    if(TARGET game-activity::game-activity_static)
        ARIEO_ENGINE_PROJECT(
            arieo_main_module
            PROJECT_TYPE module

            DEPENDENCIES
                ARIEO_PACKAGES
                    Arieo-Core
                    Arieo-Interface-Main
                    Arieo-Interface-Archive
                    Arieo-Interface-Script
                THIRDPARTY_PACKAGES
                    mimalloc
                    game-activity
                PRIVATE_LIBS
                    Arieo-Interface-Main::arieo_main_interface
                    Arieo-Interface-Archive::arieo_archive_interface
                    Arieo-Interface-Script::arieo_script_interface
                    Arieo-Core::arieo_core
                    mimalloc::mimalloc
                    game-activity::game-activity_static
                    log
                    android

            SOURCES
                CXX_SOURCES
                    ${CMAKE_CURRENT_LIST_DIR}/private/src/*.cpp
                    ${CMAKE_CURRENT_LIST_DIR}/private/src/*/*.cpp
        )
        set_target_properties(arieo_main_module PROPERTIES LINK_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")
    else()
        # game-activity not available on Android platform
        message(FATAL_ERROR "Arieo-Module-Main: Android build failed - game-activity package not found. Check CMAKE_PREFIX_PATH includes the gradle install folder.")
    endif()
else()
    # Non-Android build
    ARIEO_ENGINE_PROJECT(
        arieo_main_module
        PROJECT_TYPE module

        DEPENDENCIES
            ARIEO_PACKAGES
                Arieo-Core
                Arieo-Interface-Main
                Arieo-Interface-Archive
                Arieo-Interface-Script
            THIRDPARTY_PACKAGES
                mimalloc
            PRIVATE_LIBS
                Arieo-Interface-Main::arieo_main_interface
                Arieo-Interface-Archive::arieo_archive_interface
                Arieo-Interface-Script::arieo_script_interface
                Arieo-Core::arieo_core
                mimalloc::mimalloc

        SOURCES
            CXX_SOURCES
                ${CMAKE_CURRENT_LIST_DIR}/private/src/*.cpp
                ${CMAKE_CURRENT_LIST_DIR}/private/src/*/*.cpp
    )
endif()
