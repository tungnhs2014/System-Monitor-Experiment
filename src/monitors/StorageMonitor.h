/**
 * ============================================
 * File: src/monitors/StorageMonitor.h
 * Description: Storage monitoring - disk, swap, I/O statistics
 * ============================================
 */

#ifndef STORAGEMONITOR_H
#define STORAGEMONITOR_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QVariantList>

class StorageMonitor : public QObject 
{
    Q_OBJECT

public:
    explicit StorageMonitor(QObject *parent = nullptr);

    // Parse storage usage percentage
    int parseUsage();

    // Parse storage temperature
    int parseTemp();

    // Get/Set mount point to monitor (default: /)
    QString getMountPoint() const;
    void setMountPoint(const QString &path);

    // ==================== ROOT PARTITION ====================

    // Update roort partition statistics (call before get methods)
    void updateRootPartition();

    // Get root partition details
    QString getRootTotal() const { return m_rootTotal;}
    QString getRootUsed() const { return m_rootUsed;}
    QString getRootFree() const { return m_rootFree;}

    // ==================== SWAP ====================

    // Update swap statistics
    void updateSwap();

    // Get swap details
    int getSwapUsage() const { return m_swapUsage; }
    QString getSwapTotal() const { return m_swapTotal; }
    QString getSwapUsed() const { return m_swapUsed; }
    QString getSwapFree() const { return m_swapFree; }

    // ==================== I/O ====================
    
    // Update I/O statistics
    void updateIoStats();

    // Get current I/O rates
    QString getIoRead() const {return m_ioRead; }
    QString getIoWrite() const {return m_ioWrite; }

    // Get I/O history (last 30 values)
    QVariantList getIoHistory() const;

private:
    // ==================== STRUCTURES ====================

    // I/O statistics from /sys/block/mmcblk0/stat
    struct IoStats {
        unsigned long long sectorsRead;
        unsigned long long sectorsWritten;

        IoStats() : sectorsRead(0), sectorsWritten(0) {}
    };

    QString m_mountPoint;

    // ==================== ROOT PARTITION ====================

    QString m_rootTotal;
    QString m_rootUsed;
    QString m_rootFree;

    // ==================== SWAP ====================

    int m_swapUsage;
    QString m_swapTotal;
    QString m_swapUsed;
    QString m_swapFree;

    // ==================== I/O ====================

    // Previous I/O stats for delta calculation
    IoStats m_prevIoStats;

    // Current I/O rates
    QString m_ioRead;
    QString m_ioWrite;

    // I/O history buffer (circular, max 30 values = 60 seconds at 2s interval)
    QVector<int> m_ioHistory;
    static const int MAX_IO_HISTORY = 30;

    // ==================== HELPER METHODS ====================
    
    // Format size in bytes to human-readable string (KB, MB, GB)
    QString formatSize(unsigned long long bytes);

    // Read I/O stats from /sys/block/mmcblk0/stat
    IoStats readIoStats();

    // Calculate I/O rate in MB/s from delta
    double calculateIoRate(unsigned long long deltasetors);
};

#endif // STORAGEMONITOR_H