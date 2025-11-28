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
# C++ Standard Configuration
# ============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ============================================================================
# Qt Configuration
# ============================================================================
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 package
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Quick
    Qml
) 

# ============================================================================
# Platform Detection
# ============================================================================
message(STATUS "Platfrom Dectection")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|aarch64|armv71).*")
    set(PLATFORM_RASPBERRY_PI TRUE)
    set(PLATFORM_NAME "Raspberry Pi")
else()
    set(PLATFORM_RASPBERRY_PI FALSE)
    set(PLATFORM_NAME "Desktop")
endif()

message(STATUS "Target Platform: ${PLATFORM_NAME}")

# ============================================================================
# Source Files
# ============================================================================

# --- MODEL Layer ---
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

# --- CONTROLLER Layer  ---
set(CONTROLLER_SOURCES
    src/controller/SystemController.cpp
    src/controller/NavigationController.cpp
)

set(CONTROLLER_HEADERS
    src/controller/SystemController.h
    src/controller/NavigationController.h
)

# --- COMMON Layer ---
set(COMMON_SOURCES
    src/common/FileReader.cpp
    src/common/Logger.cpp
)

set(COMMON_HEADERS
    src/common/FileReader.h
    src/common/Logger.h
    src/common/Constants.h
)

# --- Application  ---
set(APP_SOURCES
    src/main.cpp
)

# --- Resources ---
set(RESOURCES
    resources/qml.qrc
)

# ============================================================================
# Create Executable
# ============================================================================
add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
    ${MODEL_SOURCES}
    ${MODEL_HEADERS}
    ${CONTROLLER_SOURCES}
    ${CONTROLLER_HEADERS}
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
    ${RESOURCES}
)

# ============================================================================
# Include Directories
# ============================================================================
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/model
        ${CMAKE_CURRENT_SOURCE_DIR}/src/controller
        ${CMAKE_CURRENT_SOURCE_DIR}/scr/common
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

# ============================================================================
# Link Libraries
# ============================================================================
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Quick
        Qt6::Qml

)

# ============================================================================
# Compile Definitions
# ============================================================================
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        QT_DEPRECATED_WARNINGS
        $<$<CONFIG:Debug>:QT_QML_DEBUG>
)

if(PLATFORM_RASPBERRY_PI)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            PLATFORM_RASPBERRY_PI=1
    )
else()
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            PLATFORM_DESKTOP=1
    )
endif()

# ============================================================================
# Compiler Options
# ============================================================================
target_compile_options(${PROJECT_NAME}
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        $<$<CONFIG:Release>:-O2>
        $<$<CONFIG:Debug>:-g>
)

# ============================================================================
# Installation
# ============================================================================
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install QML files for deployment
install(DIRECTORY qml/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/qml
)

# ============================================================================
# Build Summary
# ============================================================================
message(STATUS "========================================")
message(STATUS "Build Configuration Summary")
message(STATUS "========================================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Platform: ${PLATFORM_NAME}")
message(STATUS "Qt Version: ${Qt6_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "========================================")