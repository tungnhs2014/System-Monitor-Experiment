/**
 * ============================================
 * File: src/monitors/CpuMonitor.h
 * Description: CPU monitoring - usage, temperature, clock
 * ============================================
 */

#ifndef CPUMONITOR_H
#define CPUMONITOR_H

#include <QObject>
#include <QString>

class CpuMonitor : public QObject
{
    Q_OBJECT

public:
    explicit CpuMonitor(QObject *parent = nullptr);
    
    // Parse CPU usage from /proc/stat
    int parseUsage();

    // Parse CPU temperature from /sys/class/thermal
    int parseTemp();

    // Parse CPU clock from /sys/device/systems/cpu
    QString parseClock();

private:
    // Previous CPU stats for delta calculation
    unsigned long long m_prevTotal;
    unsigned long long m_prevIdle;

    // Parse /proc/stat line
    bool parseCpuStats(unsigned long long &total, unsigned long long &idle);
};

#endif // CPUMONITOR_H