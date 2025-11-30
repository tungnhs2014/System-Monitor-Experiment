# ============================================================================
# ILI9341 System Monitor
# ============================================================================
# Project: System Monitor for Raspberry Pi
# Version: 4.0.0
# ============================================================================

cmake_minimum_required(VERSION 3.16)
project(ili9341-system 
    VERSION 4.0.0
    DESCRIPTION "System Monitor with ILI9341 Display"
    LANGUAGES CXX
)

# ============================================================================
# Build Type (Default: Release)
# ============================================================================
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# ============================================================================
# C++ Standard
# ============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ============================================================================
# Qt6 Configuration
# ============================================================================
find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Qml)

# ============================================================================
# Logging Configuration
# ============================================================================
# LOG_LEVEL options:
#   0 = LOG_LEVEL_OFF      - No logging (Production/Release)
#   1 = LOG_LEVEL_CRITICAL - Critical errors only
#   2 = LOG_LEVEL_ERROR    - Errors and above
#   3 = LOG_LEVEL_WARNING  - Warnings and above
#   4 = LOG_LEVEL_INFO     - Info and above
#   5 = LOG_LEVEL_DEBUG    - All messages (Development)
#
# Usage:
#   cmake -DCMAKE_BUILD_TYPE=Release ..  → LOG_LEVEL=0 (OFF)
#   cmake -DCMAKE_BUILD_TYPE=Debug ..    → LOG_LEVEL=5 (DEBUG)
#   cmake -DLOG_LEVEL=3 ..               → Custom level (WARNING)
# ============================================================================

if(NOT DEFINED LOG_LEVEL)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(LOG_LEVEL 5)  # DEBUG - All logs
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        set(LOG_LEVEL 3)  # WARNING and above
    else()
        set(LOG_LEVEL 0)  # Release - OFF
    endif()
endif()

# Validate LOG_LEVEL
if(LOG_LEVEL LESS 0 OR LOG_LEVEL GREATER 5)
    message(FATAL_ERROR "LOG_LEVEL must be 0-5")
endif()

# Log level names for display
set(LOG_LEVEL_NAMES "OFF" "CRITICAL" "ERROR" "WARNING" "INFO" "DEBUG")
list(GET LOG_LEVEL_NAMES ${LOG_LEVEL} LOG_LEVEL_NAME)

# ============================================================================
# Platform Detection
# ============================================================================
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(PLATFORM_RASPBERRY_PI TRUE)
    set(PLATFORM_NAME "Raspberry Pi")
else()
    set(PLATFORM_DESKTOP TRUE)
    set(PLATFORM_NAME "Desktop")
endif()

# ============================================================================
# Compile Definitions
# ============================================================================
add_compile_definitions(
    # Platform
    $<$<BOOL:${PLATFORM_RASPBERRY_PI}>:PLATFORM_RASPBERRY_PI>
    $<$<BOOL:${PLATFORM_DESKTOP}>:PLATFORM_DESKTOP>
    
    # Logging
    LOG_LEVEL=${LOG_LEVEL}
    
    # Qt optimizations for Release
    $<$<CONFIG:Release>:QT_NO_DEBUG>
    $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
    $<$<CONFIG:Release>:QT_NO_INFO_OUTPUT>
    $<$<CONFIG:Release>:QT_NO_WARNING_OUTPUT>
    
    # App info
    APP_VERSION="${PROJECT_VERSION}"
)

# ============================================================================
# Include Directories
# ============================================================================
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/controller
    ${CMAKE_CURRENT_SOURCE_DIR}/src/model
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
)

# ============================================================================
# Source Files
# ============================================================================

# Common utilities
set(COMMON_SOURCES
    src/common/FileReader.cpp
    src/common/Logger.cpp
)

set(COMMON_HEADERS
    src/common/FileReader.h
    src/common/Logger.h
    src/common/Constants.h
)

# Model layer
set(MODEL_SOURCES
    src/model/CpuMonitor.cpp
    src/model/GpuMonitor.cpp
    src/model/MemoryMonitor.cpp
    src/model/StorageMonitor.cpp
    src/model/NetworkMonitor.cpp
    src/model/SettingsManager.cpp
)

set(MODEL_HEADERS
    src/model/CpuMonitor.h
    src/model/GpuMonitor.h
    src/model/MemoryMonitor.h
    src/model/StorageMonitor.h
    src/model/NetworkMonitor.h
    src/model/SettingsManager.h
)

# Controller layer
set(CONTROLLER_SOURCES
    src/controller/SystemController.cpp
    src/controller/NavigationController.cpp
)

set(CONTROLLER_HEADERS
    src/controller/SystemController.h
    src/controller/NavigationController.h
)

# Resources
set(QML_RESOURCES
    resources/qml.qrc
)

# ============================================================================
# Executable
# ============================================================================
add_executable(${PROJECT_NAME}
    src/main.cpp
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
    ${MODEL_SOURCES}
    ${MODEL_HEADERS}
    ${CONTROLLER_SOURCES}
    ${CONTROLLER_HEADERS}
    ${QML_RESOURCES}
)

# ============================================================================
# Link Libraries
# ============================================================================
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
)

# ============================================================================
# Compiler Options
# ============================================================================
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    
    # Debug: No optimization, full debug symbols
    $<$<CONFIG:Debug>:-O0>
    $<$<CONFIG:Debug>:-g3>
    $<$<CONFIG:Debug>:-DDEBUG>
    
    # Release: Maximum optimization, strip symbols
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Release>:-DNDEBUG>
    $<$<CONFIG:Release>:-ffunction-sections>
    $<$<CONFIG:Release>:-fdata-sections>
    
    # RelWithDebInfo: Optimization with debug
    $<$<CONFIG:RelWithDebInfo>:-O2>
    $<$<CONFIG:RelWithDebInfo>:-g>
    
    # MinSizeRel: Size optimization
    $<$<CONFIG:MinSizeRel>:-Os>
)

# Linker options for Release (strip unused code)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_options(${PROJECT_NAME} PRIVATE
        -Wl,--gc-sections
        -Wl,--strip-all
    )
endif()

# ============================================================================
# Installation
# ============================================================================
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ============================================================================
# Build Summary
# ============================================================================
message(STATUS "")
message(STATUS "LI9341 System Monitor - Build Config")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Platform: ${PLATFORM_NAME}")
message(STATUS "Qt Version: ${Qt6_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "══════════════════════════════════════════════════════════════")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Log Level: ${LOG_LEVEL} (${LOG_LEVEL_NAME})")
message(STATUS "══════════════════════════════════════════════════════════════")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
message(STATUS "Logging: DISABLED (Production)")
message(STATUS "Qt Debug: DISABLED")
message(STATUS "Optimize: -O3 (Maximum)")
message(STATUS "Strip: ENABLED")
else()
message(STATUS "Logging: ENABLED (Level ${LOG_LEVEL})")
message(STATUS "Qt Debug: ENABLED")
message(STATUS "Optimize: -O0/-O2 (Debug/RelWithDebInfo)")
message(STATUS "Symbols: ENABLED")
endif()