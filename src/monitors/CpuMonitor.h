/**
 * ============================================
 * File: src/monitors/CpuMonitor.h
 * Description: CPU monitoring - usage, temperature, clock, per-core, history
 * ============================================
 */

#ifndef CPUMONITOR_H
#define CPUMONITOR_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QVariantList>

class CpuMonitor : public QObject
{
    Q_OBJECT

public:
    explicit CpuMonitor(QObject *parent = nullptr);

    // ==================== MAIN METHODS ====================
    
    // Parse overall CPU usage from /proc/stat
    int parseUsage();

    // Parse CPU temperature (platform-specific)
    int parseTemp();

    // Parse CPU clock from (platform-specific)
    QString parseClock();

    // Parse per-core CPU usage (returns [core0%, core1%, core2%, core3%])
    QVariantList parsePerCoreUsage();

    // Update temperature history (call after parseTemp())
    void updateTempHistory(int temp);

    // Get temperature history for chart (returns last 40 values)
    QVariantList getTempHistory() const;

private:
    // ==================== STRUCTURES ====================
    
    // CPU statistics structure for delta calculation
    struct CoreStats {
        unsigned long long user;
        unsigned long long nice;
        unsigned long long system;
        unsigned long long idle;
        unsigned long long iowait;
        unsigned long long irq;
        unsigned long long softirq;

        CoreStats() : user(0), nice(0), system(0), idle(0),
                        iowait(0), irq(0), softirq(0) {}
    };    

    // Previous CPU stats for delta calculation
    unsigned long long m_prevTotal;
    unsigned long long m_prevIdle;

    // Previous stats for each core (4 cores on Raspberry Pi 3B+)
    QVector<CoreStats> m_prevCoreStats;

    // Temperature history buffer (circular, max 40 values = 80 seconds at 2s interval)
    QVector<int> m_tempHistory;
    static const int MAX_TEMP_HISTORY = 40;

    // ==================== HELPER METHODS ====================

    // Parse /proc/stat line for overall CPU
    bool parseCpuStats(unsigned long long &total, unsigned long long &idle);

    // Parse /proc/stat for specific core
    CoreStats readCoreStats(int coreNum);

    // Calculate usage percentage from two CoreStats
    int calculateCoreUsage(const CoreStats &prev, const CoreStats &curr);
};

#endif // CPUMONITOR_H