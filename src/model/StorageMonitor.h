/**
 * ============================================
 * File: src/model/StorageMonitor.h
 * Description: Storage/Disk monitoring and I/O statistics
 * ============================================
 */

#ifndef STORAGEMONITOR_H
#define STORAGEMONITOR_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QVariantList>

/**
 * @class StorageMonitor
 * @brief Monitors disk usage, swap, and I/O statistics
 * 
 * Reads data from:
 * - statvfs() for disk usage
 * - /proc/meminfo for swap
 * - /sys/block/device/stat for I/O statistics
 */
class StorageMonitor : public QObject 
{
    Q_OBJECT

public:
    /**
     * @brief I/O statistics structure
     */
    struct IoStats {
        unsigned long long sectorsRead = 0;
        unsigned long long sectorsWritten = 0;
    };

    explicit StorageMonitor(QObject *parent = nullptr);
    ~StorageMonitor() override = default;

    // === Disk Usage ===
    
    /**
     * @brief Parse root partition usage percentage
     * @return Usage percentage (0-100)
     */
    int parseUsage();

    /**
     * @brief Get mount point being monitored
     */
    QString getMountPoint() const;

    /**
     * @brief Set mount point to monitor
     */
    void setMountPoint(const QString &path);

     /**
     * @brief Update root partition statistics
     */
    void updateRootPartition();

    // Root partition getters
    QString getRootTotal() const { return m_rootTotal;}
    QString getRootUsed() const { return m_rootUsed;}
    QString getRootFree() const { return m_rootFree;}

    // === Swap ===

    /**
     * @brief Update swap statistics
     */
    void updateSwap();

    // Swap getters
    int getSwapUsage() const { return m_swapUsage; }
    QString getSwapTotal() const { return m_swapTotal; }
    QString getSwapUsed() const { return m_swapUsed; }
    QString getSwapFree() const { return m_swapFree; }

    // === I/O Statistics ===
    
    /**
     * @brief Update I/O statistics
     */
    void updateIoStats();

    // I/O getters
    QString getIoRead() const {return m_ioRead; }
    QString getIoWrite() const {return m_ioWrite; }

    /**
     * @brief Get I/O history for charting
     */
    QVariantList getIoHistory() const;

    // === Temperature ===

    /**
     * @brief Parse storage temperature (if available)
     * @return Temperature in Celsius, 0 if not available
     */
    int parseTemp();

private:
    /**
     * @brief Format bytes to human-readable string
     */
    QString formatSize(unsigned long long bytes);

    /**
     * @brief Read I/O statistics from /sys/block
     */
    IoStats readIoStats();

    /**
     * @brief Calculate I/O rate from sector delta
     */
    double calculateIoRate(unsigned long long deltaSectors);

    /**
     * @brief Find the primary block device
     */
    QString findBlockDevice();

private:
    // Mount point to monitor
    QString m_mountPoint;

    // Root partition stats
    QString m_rootTotal;
    QString m_rootUsed;
    QString m_rootFree;

    // Swap stats
    int m_swapUsage;
    QString m_swapTotal;
    QString m_swapUsed;
    QString m_swapFree;

    // I/O stats
    IoStats m_prevIoStats;
    QString m_ioRead;
    QString m_ioWrite;
    QVector<int> m_ioHistory;
    QString m_blockDevice;

    // Constants
    static constexpr int MAX_IO_HISTORY = 60;
    static constexpr int SECTOR_SIZE = 512;
};

#endif // STORAGEMONITOR_H