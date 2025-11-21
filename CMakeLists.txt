# ============================================
# File: CMakeLists.txt
# Description: Build configuration for desktop and Pi
# ============================================

cmake_minimum_required(VERSION 3.16)
project(ili9341-system VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Auto MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 package
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Quick
) 

# Detect target platform
if (CMAKE_CROSSCOMPILING)
    set(TARGET_PLATFORM "Pi")
    message(STATUS "Cross-compiling for Raspberry Pi")
else()
    set(TARGET_PLATFORM "Desktop")
    message(STATUS "Building for Desktop")
endif()

# Source files
set(SOURCES
    main.cpp
    src/SystemInfo.cpp
    src/monitors/CpuMonitor.cpp
    src/monitors/GpuMonitor.cpp
    src/monitors/MemoryMonitor.cpp
    src/monitors/StorageMonitor.cpp
    src/monitors/NetworkMonitor.cpp
    src/utils/FileReader.cpp
)

# Header files
set(HEADERS
    src/SystemInfo.h
    src/monitors/CpuMonitor.h
    src/monitors/GpuMonitor.h
    src/monitors/MemoryMonitor.h
    src/monitors/StorageMonitor.h
    src/monitors/NetworkMonitor.h
    src/utils/FileReader.h
)

# Qt resources
set(RESOURCES
    resources/qml.qrc
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Quick

)

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/monitors
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
)

# Compiler definitions
if(TARGET_PLATFORM STREQUAL "Pi")
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            TARGET_PI
    )
else()
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            TARGET_DESKTOP
    )
endif()

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Print build info
message(STATUS "==========================================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Target Platfrom: ${TARGET_PLATFORM}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Qt Version: ${Qt6_VERSION}")