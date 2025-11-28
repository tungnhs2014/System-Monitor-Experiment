/**
 * ============================================================================
 * File: src/common/Constants.h
 * Description: Application-wide constants and configuration
 * ============================================================================
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QColor>

namespace App {

// ============================================================================
// Application Info
// ============================================================================
namespace Info {
    constexpr const char* NAME = "ILI9341 System Monitor";
    constexpr const char* VERSION = "4.0.0";
    constexpr const char* ORGANIZATION = "ILI9341";
    constexpr const char* DOMAIN = "ili9341.embedded";
}

// ============================================================================
// System Paths (Linux)
// ============================================================================

namespace Path {
    // CPU
    constexpr const char* PROC_STAT = "/proc/stat";
    constexpr const char* CPU_TEMP = "/sys/class/thermal/thermal_zone0/temp";
    constexpr const char* CPU_FREQ = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq";

    // Memory
    constexpr const char* PROC_MEMINFO = "/proc/meminfo";
}

}

#endif // CONSTANTS_H