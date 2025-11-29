/**
 * ============================================
 * File: src/model/MemoryMonitor.h
 * Description: RAM monitoring and data collection
 * ============================================
 */

#ifndef MEMORYMONITOR_H
#define MEMORYMONITOR_H

#include <QObject>
#include <QString>
#include <QMap>

/**
 * @class MemoryMonitor
 * @brief Monitors RAM usage, swap, and cache statistics
 * 
 * Read data from /proc/meminfo
 */
class MemoryMonitor : public QObject 
{
    Q_OBJECT

public:
    explicit MemoryMonitor(QObject *parent = nullptr);
    ~MemoryMonitor() override = default;

    /**
     * @brief Parse RAM usage percentage
     * @return Usage percentage (0-100)
     */
    int parseUsage();

    /**
     * @brief Parse used RAM
     * @return Used RAM string (e.g., "512M")
     */
    QString parseUsed();

    /**
     * @brief Parse free/available RAM
     * @return Free RAM string (e.g., "256M")
     */
    QString parseFree();

    /**
     * @brief Parse total RAM in MB
     * @return Total RAM in MB
     */
    int parseTotal();

    /**
     * @brief Parse cached memory
     * @return Cached memory string (e.g., "128M")
     */
    QString parseCache();

    /**
     * @brief Parse buffer memory
     * @return Buffers memory string
     */
    QString parseBuffers();

    /**
     * @brief Get raw memory values, for detailed display
     */
    unsigned long long getUsedBytes();
    unsigned long long getFreeBytes();
    unsigned long long getTotalBytes();
    unsigned long long getCachedBytes();
    unsigned long long getBuffersBytes();

private:
    /**
     * @brief Parse /proc/meminfo into key-value map
     * @return Map of memory metrics (values in kB)
     */
    QMap<QString, unsigned long long> parseMemInfo();

    /**
     * @brief Convert bytes to human-readable MB string
     */
    QString bytesToMB(unsigned long long bytes);

    /**
     * @brief Covert bytes to human-readable string with auto unit
     */
    QString bytesToHuman(unsigned long long bytes);

private:
    // Cached meminfo for mutiple queries in same update cycle
    QMap<QString, unsigned long long> m_cachedMemInfo;
    qint64 m_lastCacheTime;

    static constexpr int CACHE_VALIDITY_MS = 100; // Cache valid for 100ms
};

#endif // MEMORYMONITOR_H