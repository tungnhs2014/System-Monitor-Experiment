# ============================================
# File: CMakeLists.txt
# Description: Build configuration for desktop and Pi
# ============================================

cmake_minimum_required(VERSION 3.16)
project(ili9341-system VERSION 3.1.0 LANGUAGES CXX)

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

# ==================== PLATFORM DETECTION ====================

message(STATUS "Detecting platform.CMAKE_SYSTEM_PROCESSOR..")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|aarch64|armv71).*")
    set(PLATFORM_RASPBERRY_PI TRUE)
    message(STATUS "Detected: Raspberry Pi (ARM processor)")
else()
    set(PLATFORM_RASPBERRY_PI FALSE)
    message(STATUS "Detected: Desktop/Ubuntu (x86/64 processor)")
endif()

# ==================== SOURCE FILES ====================

set(SOURCES
    main.cpp
    src/SystemInfo.cpp
    src/SettingsManager.cpp
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
    src/SettingsManager.h
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

# ==================== CREATE EXECUTABLE ====================

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# ==================== LINK LIBRARIES ====================

# Link Qt libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Quick

)

# ==================== INCLUDE DIRECTORIES ===================

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/monitors
        ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
)

# ==================== COMPILER DEFINITIONS ====================

if(PLATFORM_RASPBERRY_PI)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            PLATFORM_RASPBERRY_PI=1
    )
    message(STATUS "Compile flag: PLATFORM_RASPBERRY_PI=1")
else()
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            PLATFORM_DESKTOP=1
    )
    message(STATUS "Compile flag: PLATFORM_DESKTOP=1")
endif()

# ==================== INSTALL TARGET ====================

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# ==================== BUILD SUMMARY ====================
message(STATUS "==========================================")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Target: ${PLATFORM_RASPBERRY_PI}")

if(PLATFORM_RASPBERRY_PI)
    message(STATUS "Build for: Raspberry Pi")
else()
    message(STATUS "Build for: Desktop/Ubuntu")
endif()

message(STATUS "Qt Version: ${Qt6_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")