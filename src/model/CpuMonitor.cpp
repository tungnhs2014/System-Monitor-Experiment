/**
 * ============================================
 * File: src/monitors/CpuMonitor.cpp
 * Description: CPU monitoring implementation
 * ============================================
 */

#include "CpuMonitor.h"
#include "FileReader.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>

CpuMonitor::CpuMonitor(QObject *parent) 
    : QObject(parent)
    , m_prevTotal(0)
    , m_prevIdle(0)
{
    // Initialize with 4 cores for Raspberry Pi 3B+
    m_prevCoreStats.resize(4);

    // Reserve space for temperature history
    m_tempHistory.reserve(MAX_TEMP_HISTORY);

    qDebug() << "CpuMonitor initialized - 4 cores, temp history buffer:" << MAX_TEMP_HISTORY;

#ifdef PLATFORM_RASPBERRY_PI
    qDebug() << "Platform Rasberry Pi (compile-time detected)";
#else
    qDebug() << "Platform: Desktop/Ubuntu (compile-time detected)";
#endif
}

int CpuMonitor::parseUsage()
{
    unsigned long long total, idle;

    if (!parseCpuStats(total, idle)) {
        qWarning() << "Failed to parse CPU stats";
        return 0;
    }

    // First call - initialize previous values
    if (m_prevTotal == 0) {
        m_prevTotal = total;
        m_prevIdle = idle;
        return 0;
    }

    // Calculate deltas
    unsigned long long totalDelta = total - m_prevTotal;
    unsigned long long idleDelta = idle - m_prevIdle;

    // Update previous values
    m_prevTotal = total;
    m_prevIdle = idle;

    // Calculate usage percentage
    if (totalDelta == 0) {
        return 0;
    }

    int usage = 100 * (totalDelta - idleDelta) / totalDelta;
    return qBound(0, usage, 100);
}

int CpuMonitor::parseTemp()
{
    // Platform-specific temperature reading
#ifdef PLATFORM_RASPBERRY_PI
    // Raspberry Pi: Always use thermal_zone0
    QString tempStr = FileReader::readFile("/sys/class/thermal/thermal_zone0/temp");

    if (tempStr.isEmpty()) {
        // Only warn once
        static bool warned = false;
        if (!warned) {
            qWarning() << "Failed to read Raspberry Pi temperature sensor";
            warned = true;
        }
        return 0;
    }
    
    bool ok;
    int tempMilliDegrees = tempStr.toInt(&ok);

    if (!ok) {
        return 0;
    }

    // Convert from millidegrees to degrees
    return tempMilliDegrees / 1000;

#else
    // Desktop/Ubuntu: Try multiple sources

    // Try hwmon (coretemp on Ubuntu)
    QStringList hwmonPaths = {
        "/sys/class/hwmon/hwmon0/temp1_input",
        "/sys/class/hwmon/hwmon1/temp1_input",
        "/sys/class/hwmon/hwmon2/temp1_input",
        "/sys/class/thermal/thermal_zone0/temp"  // Fallback
    };
    
    for (const QString &path: hwmonPaths) {
        if (!FileReader::fileExits(path)) {
            continue;
        }

        QString tempStr = FileReader::readFile(path);
        if (tempStr.isEmpty()) {
            continue;
        }

        bool ok;
        int tempValue = tempStr.toInt(&ok);
        if (!ok) {
            continue;
        }

        // Found working sensor
        static bool firstRead = true;
        if (firstRead) {
            qDebug() << "Temperature sensor found at: " << path;
            firstRead = false;
        }

        // Convert from millidegrees to degrees
        return tempValue / 1000;
    }

    // No sensor found - this is normal on VMs
    static bool warnedNoSensor = false;
    if (!warnedNoSensor) {
        qWarning() << "No temperature sensor found (normal on VMs)";
        warnedNoSensor = true;
    }
    return 0;
#endif
}

QString CpuMonitor::parseClock()
{
    // Platform-specific clock reading
#ifdef PLATFORM_RASPBERRY_PI
    // Raspberry Pi: Always use scaling_cur_freq
    QString freqStr = FileReader::readFirstLine("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
    
    if (freqStr.isEmpty()) {
        return "N/A";
    }

    bool ok;
    int freqKHz = freqStr.toInt(&ok);

    if (!ok) {
        return "N/A";
    }

    // Convert to GHz
    double freqGHz = freqKHz / 1000000.0;
    return QString::number(freqGHz, 'f', 1) + "G";
#else 
     // Desktop/Ubuntu: Try multiple sources
    QStringList clockPaths = {
        "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq",
        "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq"
    };
    
    for (const QString &path : clockPaths) {
        QString freqStr = FileReader::readFirstLine(path);
        if (freqStr.isEmpty()) {
            continue;
        }
        
        bool ok;
        int freqKHz = freqStr.toInt(&ok);
        if (!ok) {
            continue;
        }
        
        // Found working sensor
        double freqGHz = freqKHz / 1000000.0;
        return QString::number(freqGHz, 'f', 1) + "G";
    }

    // No frequency info (normal on some systems)
    static bool warnedNoClock = false;
    if (!warnedNoClock) {
        qWarning() << "CPU frequency not available (normal on VMs/fixed frequency systems)";
        warnedNoClock = true;
    }
    return "N/A";
#endif
}

QVariantList CpuMonitor::parsePerCoreUsage()
{
    QVariantList result;

    // Parse usage for each of 4 cores
    for (int i = 0; i < 4; i++) {
        CoreStats currentStats = readCoreStats(i);

        // Calculate usage from previous and current stats
        int usage = calculateCoreUsage(m_prevCoreStats[i], currentStats);

        // Store current stats for next calculation
        m_prevCoreStats[i] = currentStats;

        // Add to result list
        result.append(usage);
    }
    return result;
}

void CpuMonitor::updateTempHistory(int temp)
{
    // Add new temperature to history
    m_tempHistory.append(temp);

    // Keep only last MAX_TEMP_HISTORY values (circular buffer behavior)
    if (m_tempHistory.size() > MAX_TEMP_HISTORY) {
        m_tempHistory.removeFirst();
    }
}

QVariantList CpuMonitor::getTempHistory() const
{
    QVariantList result;

    // Convert QVector<int> to QVariantList for QML
    for (int temp : m_tempHistory) {
        result.append(temp);
    }

    return result;
}

bool CpuMonitor::parseCpuStats(unsigned long long &total, unsigned long long &idle)
{
    // Read /proc/stat
    QStringList lines = FileReader::readLines("/proc/stat");

    if (lines.isEmpty()) {
        return false;
    }

    // First line format: "cpu user nice system idle iowait irq softirq ..."
    QString cpuLine = lines.first();

    if (!cpuLine.startsWith("cpu ")) {
        return false;
    }

    // Split and parse values
    QStringList parts = cpuLine.split(' ', Qt::SkipEmptyParts);

    if (parts.size() < 5) {
        return false;
    }

    // Parse CPU times
    unsigned long long user = parts[1].toLongLong();
    unsigned long long nice = parts[2].toLongLong();
    unsigned long long system = parts[3].toLongLong();
    unsigned long long idleTime  = parts[4].toLongLong();
    unsigned long long iowait = (parts.size() > 5) ? parts[5].toLongLong() : 0;
    unsigned long long irq = (parts.size() > 6) ? parts[6].toLongLong() : 0;
    unsigned long long softirq  = (parts.size() > 7) ? parts[7].toLongLong() : 0;

    // Calculate total and idle
    total = user + nice + system + idleTime + iowait + irq + softirq;
    idle = idleTime + iowait;
    
    return true;
}

CpuMonitor::CoreStats CpuMonitor::readCoreStats(int coreNum)
{
    CoreStats stats;

    // Read /proc/stat file (same on all platforms)
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to opennn /proc/stat";
        return stats;
    }

    QTextStream in(&file);
    QString line;
    QString searchStr = QString("cpu%1 ").arg(coreNum);

    // Search for the line starting with "cpu0 ", "cpu1 ", etc.
    while (!in.atEnd()) {
        line = in.readLine();

        if (line.startsWith(searchStr)) {
            // Parse the line
            // Format: "cpu0 user nice system idle iowait irq softirq steal guest guest_nice"
            QStringList parts = line.split(' ', Qt::SkipEmptyParts);

            if (parts.size() >= 8) {
                stats.user = parts[1].toULongLong();
                stats.nice = parts[2].toULongLong();
                stats.system = parts[3].toULongLong();
                stats.idle = parts[4].toULongLong();
                stats.iowait = parts[5].toULongLong();
                stats.irq = parts[6].toULongLong();
                stats.softirq = parts[7].toULongLong();
            }
            break;
        }
    }

    file.close();
    return stats;
}

int CpuMonitor::calculateCoreUsage(const CoreStats &prev, const CoreStats &curr)
{
    // Calculate total time for previous reading
     unsigned long long prevTotal = prev.user + prev.nice + prev.system + 
                                    prev.idle + prev.iowait + prev.irq + prev.softirq;
    
    // Calculate total time for current reading
    unsigned long long currTotal = curr.user + curr.nice + curr.system + 
                                    curr.idle + curr.iowait + curr.irq + curr.softirq;
    
    // Calculate idle time
    unsigned long long prevIdle = prev.idle + prev.iowait;
    unsigned long long currIdle = curr.idle + curr.iowait;
    
    // Calculate deltas
    unsigned long long deltaTotal = currTotal - prevTotal;
    unsigned long long deltaIdle = currIdle - prevIdle;
    
    // Avoid division by zero
    if (deltaTotal == 0) {
        return 0;
    }
    
    // Calculate usage percentage
    // Usage = (Total - Idle) / Total * 100
    int usage = (100 * (deltaTotal - deltaIdle)) / deltaTotal;
    
    // Clamp to 0-100 range
    return qBound(0, usage, 100);
}
