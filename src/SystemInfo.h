/**
 * ============================================
 * File: src/SystemInfo.h
 * Description: Main system info aggregator
 * ============================================
 */

#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <QObject>
#include <QTimer>
#include "monitors/CpuMonitor.h"
#include "monitors/GpuMonitor.h"
#include "monitors/MemoryMonitor.h"
#include "monitors/StorageMonitor.h"
#include "monitors/NetworkMonitor.h"

class SystemInfo : public QObject 
{
    Q_OBJECT

    // CPU Properties
    Q_PROPERTY(int cpuUsage READ cpuUsage NOTIFY cpuUsageChanged)
    Q_PROPERTY(int cpuTemp READ cpuTemp NOTIFY cpuTempChanged)
    Q_PROPERTY(QString cpuClock READ cpuClock NOTIFY cpuClockChanged)

    // GPU Properties
    Q_PROPERTY(int gpuUsage READ gpuUsage NOTIFY gpuUsageChanged)
    Q_PROPERTY(int gpuTemp READ gpuTemp NOTIFY gpuTempChanged)
    Q_PROPERTY(int gpuMemUsage READ gpuMemUsage NOTIFY gpuMemUsageChanged)

    // RAM Properties
    Q_PROPERTY(int ramUsage READ ramUsage NOTIFY ramUsageChanged)
    Q_PROPERTY(QString ramUsed READ ramUsed NOTIFY ramUsedChanged)
    Q_PROPERTY(QString ramFree READ ramFree NOTIFY ramFreeChanged)

    // Storage Properties
    Q_PROPERTY(int hddUsage READ hddUsage NOTIFY hddUsageChanged)
    Q_PROPERTY(int hddTemp READ hddTemp NOTIFY hddTempChanged)

    // Network Properties
    Q_PROPERTY(QString netDown READ netDown NOTIFY netDownChanged)
    Q_PROPERTY(QString netUp READ netUp NOTIFY netUpChanged)

public:
    explicit SystemInfo(QObject *parent = nullptr);

    // CPU Getters
    int cpuUsage() const { return m_cpuUsage; }
    int cpuTemp() const { return m_cpuTemp; }
    QString cpuClock() const { return m_cpuClock; }

    // GPU Getters
    int gpuUsage() const { return m_gpuUsage; }
    int gpuTemp() const { return m_gpuTemp; }
    int gpuMemUsage() const { return m_gpuMemUsage; }

    // RAM Getters
    int ramUsage() const { return m_ramUsage; }
    QString ramUsed() const { return m_ramUsed; }
    QString ramFree() const { return m_ramFree; }

    // Storage Getters
    int hddUsage() const { return m_hddUsage; }
    int hddTemp() const { return m_hddTemp; }

    // Network Getters
    QString netDown() const { return m_netDown; }
    QString netUp() const { return m_netUp; }

signals:
    // CPU Signals
    void cpuUsageChanged();
    void cpuTempChanged();
    void cpuClockChanged();

    // GPU Signals
    void gpuUsageChanged();
    void gpuTempChanged();
    void gpuMemUsageChanged();

    // RAM Signals
    void ramUsageChanged();
    void ramUsedChanged();
    void ramFreeChanged();

    // Storage Signals
    void hddUsageChanged();
    void hddTempChanged();

    // Network Signals
    void netDownChanged();
    void netUpChanged();

private slots:
    void updateAllMetrics();

private:
    // Monitor instances
    CpuMonitor *m_cpuMonitor;
    GpuMonitor *m_gpuMonitor;
    MemoryMonitor *m_memMonitor;
    StorageMonitor *m_storageMonitor;
    NetworkMonitor *m_networkMonitor;

    // Update timer
    QTimer *m_updateTimer;

    // CPU Values
    int m_cpuUsage;
    int m_cpuTemp;
    QString m_cpuClock;

    // GPU values
    int m_gpuUsage;
    int m_gpuTemp;
    int m_gpuMemUsage;

    // RAM Values
    int m_ramUsage;
    QString m_ramUsed;
    QString m_ramFree;

    // Storage Values
    int m_hddUsage;
    int m_hddTemp;

    // Network Values
    QString m_netDown;
    QString m_netUp;

    // Helper methods
    void setCpuUsages(int value);
    void setCpuTemp(int value);
    void setCpuClock(const QString &value);

    void setGpuUsages(int value);
    void setGpuTemp(int value);
    void setGpuMemUsage(int value);

    void setRamUsage(int value);
    void setRamUsed(const QString &value);
    void setRamFree(const QString &value);

    void setHddUsage(int value);
    void setHddTemp(int value);

    void setNetDown(const QString &value);
    void setNetUp(const QString &value);
};

#endif // SYSTEMINFO_H
