/**
 * ============================================
 * File: src/model/CpuMonitor.cpp
 * Description: CPU monitoring implementation
 * ============================================
 */

#include "CpuMonitor.h"
#include "FileReader.h"
#include "Constants.h"
#include "Logger.h"
#include <QFile>
#include <QTextStream>
#include <QDir>

CpuMonitor::CpuMonitor(QObject *parent) 
    : QObject(parent)
    , m_prevTotal(0)
    , m_prevIdle(0)
    , m_coreCount(DEFAULT_CORE_COUNT)
{
    // Detect number of cores
    QDir cpuDir("/sys/devices/system/cpu");
    QStringList cpuDirs = cpuDir.entryList(QStringList() << "cpu[0-9]*", QDir::Dirs);
    if (!cpuDirs.isEmpty()) {
        m_coreCount = cpuDirs.size();
    }

    // Initalize per-core stats
    m_prevCoreStats.resize(m_coreCount);

    // Reserve temperature history buffer
    m_tempHistory.reserve(MAX_TEMP_HISTORY);
    
    // Find temperartue sensor path

        LOG_INFO(QString("CpuMonitor initialized - %1 cores, temp sensor: %2")
             .arg(m_coreCount)
             .arg(m_tempSensorPath.isEmpty() ? "not found" : m_tempSensorPath));
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

    int usage = static_cast<int>(100 * (totalDelta - idleDelta) / totalDelta);
    return qBound(0, usage, 100);
}

int CpuMonitor::parseTemp()
{
    if (m_tempSensorPath.isEmpty()) {
        return 0;
    }

    int tempMilliDegrees = FileReader::readInt(m_tempSensorPath, 0);

    if (tempMilliDegrees == 0) {
        return 0;
    }

    // Convert from millidegrees to degrees Celsius
    return tempMilliDegrees / 1000;
}

QString CpuMonitor::parseClock()
{
    // Try multiple paths for CPU frequency
    QStringList freqPaths = {
        "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq",
        "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq"
    };
    
    for (const QString &path : freqPaths) {
        if (!FileReader::fileExists(path)) {
            continue;
        }
        
        int freqKHz = FileReader::readInt(path, 0);
        if (freqKHz > 0) {
            // Convert tp GHz with 1 decimal place
            double freqGHz = freqKHz / 1000000.0;
            return QString::number(freqGHz, 'f', 1) + "G";
        }
    }
    
    return "N/A";
}

QVariantList CpuMonitor::parsePerCoreUsage()
{
    QVariantList result;
    
    for (int i = 0; i < m_coreCount; ++i) {
        CoreStats currentStats = readCoreStats(i);
        int usage = calculateCoreUsage(m_prevCoreStats[i], currentStats);
        m_prevCoreStats[i] = currentStats;
        result.append(usage);
    }
    
    return result;
}

void CpuMonitor::updateTempHistory(int temp)
{
    m_tempHistory.append(temp);

    // Maintain circular buffer behavior
    if (m_tempHistory.size() > MAX_TEMP_HISTORY) {
        m_tempHistory.removeFirst();
    }
}

QVariantList CpuMonitor::getTempHistory() const
{
    QVariantList result;
    result.reserve(m_tempHistory.size());

    // Convert QVector<int> to QVariantList for QML
    for (int temp : m_tempHistory) {
        result.append(temp);
    }

    return result;
}

QString CpuMonitor::parseLoadAverage()
{
    QString loadStr = FileReader::readFirstLine("/proc/loadavg");

    if (loadStr.isEmpty()) {
        return "N/A";
    }

    // Format: "0.00 0.01 0.05 1/234 5678"
    // We want first 3 values
    QStringList parts = loadStr.split(' ', Qt::SkipEmptyParts);

    if (parts.size() >= 3) {
        return QString("%1 %2 %3").arg(parts[0], parts[1], parts[2]);
    }

    return "N/A";
}

bool CpuMonitor::parseCpuStats(unsigned long long &total, unsigned long long &idle)
{
    QStringList lines = FileReader::readLines(App::Path::PROC_STAT);

    if (lines.isEmpty()) {
        return false;
    }

    // First line format: "cpu user nice system idle iowait irq softirq ..."
    QString cpuLine = lines.first();

    if (!cpuLine.startsWith("cpu ")) {
        return false;
    }

    QStringList parts = cpuLine.split(' ', Qt::SkipEmptyParts);

    if (parts.size() < 5) {
        return false;
    }

    // Parse CPU times (skip "cpu" label at index 0)
    unsigned long long user = parts[1].toULongLong();
    unsigned long long nice = parts[2].toULongLong();
    unsigned long long system = parts[3].toULongLong();
    unsigned long long idleTime = parts[4].toULongLong();
    unsigned long long iowait = (parts.size() > 5) ? parts[5].toULongLong() : 0;
    unsigned long long irq = (parts.size() > 6) ? parts[6].toULongLong() : 0;
    unsigned long long softirq = (parts.size() > 7) ? parts[7].toULongLong() : 0;

    // Calculate total and idle
    total = user + nice + system + idleTime + iowait + irq + softirq;
    idle = idleTime + iowait;
    
    return true;
}

CpuMonitor::CoreStats CpuMonitor::readCoreStats(int coreNum)
{
    CoreStats stats;

    QFile file(App::Path::PROC_STAT);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return stats;
    }

    QTextStream stream(&file);
    QString searchStr = QString("cpu%1 ").arg(coreNum);

    // Search for the line starting with "cpu0 ", "cpu1 ", etc.
    while (!stream.atEnd()) {
        QString line = stream.readLine();

        if (line.startsWith(searchStr)) {
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
    int usage = static_cast<int>((100 * (deltaTotal - deltaIdle)) / deltaTotal);
    return qBound(0, usage, 100);
}

QString CpuMonitor::findTempSensorPath()
{
    // Priority list of temperature sensor paths
    QStringList sensorPaths = {
        "/sys/class/thermal/thermal_zone0/temp",           // Raspberry Pi, most Linux
        "/sys/class/hwmon/hwmon0/temp1_input",             // Intel coretemp
        "/sys/class/hwmon/hwmon1/temp1_input",
        "/sys/class/hwmon/hwmon2/temp1_input",
        "/sys/devices/virtual/thermal/thermal_zone0/temp"  // Fallback
    };

    for (const QString& path: sensorPaths) {
        if (FileReader::fileExists(path)) {
            return path;
        }
    }

    LOG_WARNING("No temperature sensor found");
    QString();
}
