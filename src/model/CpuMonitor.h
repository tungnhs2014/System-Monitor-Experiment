/**
 * ============================================================================
 * File: src/model/CpuMonitor.h
 * Description: CPU monitoring and data collection
 * ============================================================================
 */

#ifndef CPUMONITOR_H
#define CPUMONITOR_H

#include <QObject>
#include <QVector>
#include <QVariantList>

/**
 * @class CpuMonitor
 * @brief Monitors CPU usage, temperature, frequency, and per-core statistics
 * 
 * Reads data from:
 * - /proc/stat (CPU usage)
 * - /sys/class/thermal/thermal_zone0/temp (temperature)
 * - /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq (frequency)
 */
class CpuMonitor : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Structure to hold per-core CPU statistics
     */
    struct CoreStats {
        unsigned long long user = 0;
        unsigned long long nice = 0;
        unsigned long long system = 0;
        unsigned long long idle = 0;
        unsigned long long iowait = 0;
        unsigned long long irq = 0;
        unsigned long long softirq = 0;
    };
    explicit CpuMonitor(QObject *parent = nullptr);
    ~CpuMonitor() override = default;
    
    /**
     * @brief Parse overall CPU usage percentage
     * @return CPU usage (0-100)
     */
    int parseUsage();

    /**
     * @brief Parse CPU temperature
     * @return Temperature in Celsius
     */
    int parseTemp();

    /**
     * @brief Parse CPU clock frequency
     * @return Frequency string (e.g., "1.4G")
     */
    QString parseClock();

    /**
     * @brief Parse per-core CPU usage
     * @return List of usage percentages for each core
     */
    QVariantList parsePerCoreUsage();

    /**
     * @brief Update temperature history buffer
     * @param temp Current temperature
     */
    void updateTempHistory(int temp);

    /**
     * @brief Get temperature history for charting
     * @return List of temperature values
     */
    QVariantList getTempHistory() const;

    /**
     * @brief Parse load average
     * @return Load average string (1, 5, 15 min)
     */
    QString parseLoadAverage();

    /**
     * @brief Get number of CPU cores
     * @return Core count
     */
    int getCoreCount() const { return m_coreCount; }

private:
    /**
     * @brief Parse CPU stats from /proc/stat
     */
    bool parseCpuStats(unsigned long long& total, unsigned long long& idle);

    /**
     * @brief Read statistics for a specific core
     */
    CoreStats readCoreStats(int coreNum);

    /**
     * @brief Calculate usage from previous and current stats
     */
    int calculateCoreUsage(const CoreStats& prev, const CoreStats& curr);

    /**
     * @brief Find available temparature sensor path 
     */
    QString findTempSensorPath();

private:
    // Previous readings for delta calculation
    unsigned long long m_prevTotal;
    unsigned long long m_prevIdle;
    
    // Per-core previous statistics
    QVector<CoreStats> m_prevCoreStats;

    // Temperature history for charting
    QVector<int> m_tempHistory;

    // Configuration
    int m_coreCount;
    QString m_tempSensorPath;

    // Constants
    static constexpr int MAX_TEMP_HISTORY = 60;
    static constexpr int DEFAULT_CORE_COUNT = 4;
};

#endif // CPUMONITOR_H