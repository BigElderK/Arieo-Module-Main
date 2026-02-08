cmake_minimum_required (VERSION 3.31)

if(ARIEO_HOST_OS STREQUAL "android")
    arieo_engine_project(
        arieo_main_module
        ALIAS arieo::modules::main
        PROJECT_TYPE module
        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*/*.cpp
        PACKAGES
            mimalloc
            game-activity
        INTERFACES
            arieo::interface::main
            arieo::interface::archive
            arieo::interface::script
        LIBS
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
            mimalloc
        INTERFACES
            arieo::interface::main
            arieo::interface::archive
            arieo::interface::script
        LIBS
            arieo::base
            arieo::core
            mimalloc::mimalloc
    )
endif()