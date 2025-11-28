/**
 * ============================================
 * File: src/monitors/GpuMonitor.h
 * Description: GPU monitoring with fallback (no vcgencmd)
 * ============================================
 */

#ifndef GPUMONITOR_H
#define GPUMONITOR_H

#include <QObject>
#include <QString>

class GpuMonitor : public QObject
{
    Q_OBJECT

public:
    explicit GpuMonitor(QObject *parent = nullptr);

    // Parse GPU temperature (Fallback to thermal_zone0)
    int parseTemp();

    // Parse GPU memory usage (return -1 if not avaiable)
    int parseMemUsage();

    // Check if vcgencmd is available
    bool isVcgencmdAvailable();

private:
    bool m_vcgencmdAvailable;
};

#endif // GPUMONITOR_H