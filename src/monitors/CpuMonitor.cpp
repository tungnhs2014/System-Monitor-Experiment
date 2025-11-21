/**
 * ============================================
 * File: src/monitors/CpuMonitor.cpp
 * Description: CPU monitoring implementation
 * ============================================
 */

#include "CpuMonitor.h"
#include "FileReader.h"
#include <QDebug>

CpuMonitor::CpuMonitor(QObject *parent) 
    : QObject(parent)
    , m_prevTotal(0)
    , m_prevIdle(0)
{
}

int CpuMonitor::parseUsage()
{
    unsigned long long total, idle;

    if (!parseCpuStats(total, idle)) {
        qWarning() << "Failed to parse stats";
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
    // Raspberry Pi: /sys/class/thermal/thermal_zone0/temp
    // Returns temperature in millidegrees Celsius

    QString tempStr = FileReader::readFile("/sys/class/thermal/thermal_zone0/temp");

    if (tempStr.isEmpty()) {
        qWarning() << "Failed to read CPU temperature";
        return 0;
    }
    
    bool ok;
    int tempMilliDegrees = tempStr.toInt(&ok);

    if (!ok) {
        qWarning() << "Failed to parse CPU temperature:" << tempStr;
        return 0;
    }

    // Convert from millidegrees to degrees
    return tempMilliDegrees / 1000;
}

QString CpuMonitor::parseClock()
{
    // Read CPU frequency in KHz
    QString freStr = FileReader::readFirstLine("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");

    if (freStr.isEmpty()) {
        return "N/A";
    }

    bool ok;
    int freqKHz = freStr.toInt(&ok);

    if (!ok) {
        return "N/A";
    }

    // Convert to GHz
    double freqGHz = freqKHz / 1000000.0;
    return QString::number(freqGHz, 'f', 1) + "G";
}

bool CpuMonitor::parseCpuStats(unsigned long long &total, unsigned long long &idle)
{
    // Read /proc/stat
    QStringList lines = FileReader::readLines("/proc/stat");

    if (lines.isEmpty()) {
        return false;
    }

    // First line format: "cpu user nice system idle iowait irq softirq ..."
    QString cpuline = lines.first();

    if (!cpuline.startsWith("cpu ")) {
        return false;
    }

    // Split and parse values
    QStringList parts = cpuline.split(' ', Qt::SkipEmptyParts);

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
    total = user + nice + system + iowait + irq + softirq;
    idle = idleTime + iowait;
    
    return true;
}
