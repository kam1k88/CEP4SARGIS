cmake_minimum_required(VERSION 3.16)
project(SargisLab VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(SargisLab_USE_QCUSTOMPLOT "Use QCustomPlot for plotting (otherwise Qt Charts)" ON)
option(SargisLab_BUILD_PLUGINS "Build example plugins" ON)

# Platform detection
if(WIN32)
    set(SargisLab_PLATFORM_WIN 1)
elseif(APPLE)
    set(SargisLab_PLATFORM_MACOS 1)
endif()

# Dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(cmake/Dependencies.cmake)

# Subprojects
add_subdirectory(platform)
add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(app)

if(SargisLab_BUILD_PLUGINS)
    add_subdirectory(plugins)
endif()

# Tests
enable_testing()
add_subdirectory(tests)

# Install (macOS bundle)
if(APPLE)
    install(TARGETS SargisLab BUNDLE DESTINATION .)
    if(EXISTS "${CMAKE_SOURCE_DIR}/resources/icons/app.icns")
        install(FILES resources/icons/app.icns
            DESTINATION SargisLabScientificApp.app/Contents/Resources)
    endif()
elseif(WIN32)
    install(TARGETS SargisLab RUNTIME DESTINATION bin)
endif()
