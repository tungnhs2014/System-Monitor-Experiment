/**
 * ============================================
 * File: src/model/StorageMonitor.cpp
 * Description: Storage monitoring implementation
 * ============================================
 */

 #include "StorageMonitor.h"
 #include "FileReader.h"
 #include "Constants.h"
 #include "Logger.h"
 #include <sys/statvfs.h>
 #include <QDir>

StorageMonitor::StorageMonitor(QObject *parent)
    : QObject(parent)
    , m_mountPoint("/")
    , m_rootTotal("0GB")
    , m_rootUsed("0GB")
    , m_rootFree("0GB")
    , m_swapUsage(0)
    , m_swapTotal("0M")
    , m_swapUsed("0M")
    , m_swapFree("0M")
    , m_ioRead("0.0 MB/s")
    , m_ioWrite("0.0 MB/s")
{
    m_ioHistory.reserve(MAX_IO_HISTORY);
    m_blockDevice = findBlockDevice();

    LOG_INFO(QString("StorageMonitor initialized - mount: %1, device: %2").arg(m_mountPoint, m_blockDevice));
}

int StorageMonitor::parseUsage()
{
    struct statvfs stat;

    if (statvfs(m_mountPoint.toUtf8().constData(), &stat) != 0) {
        return 0;
    }

    // Calculate usage
    unsigned long long total = stat.f_blocks * stat.f_frsize;
    unsigned long long available = stat.f_bavail * stat.f_frsize;
    unsigned long long used = total - available;

    if (total == 0) {
        return 0;
    }

    int usage = static_cast<int>((used * 100) / total);
    return qBound(0, usage, 100);
}

int StorageMonitor::parseTemp()
{
    // Disk temperature typically not available on Pi
    // Could use hdparm or smartctl on desktop
    return 0;
}

QString StorageMonitor::getMountPoint() const
{
    return m_mountPoint;
}

void StorageMonitor::setMountPoint(const QString &path)
{
    if(m_mountPoint != path) {
        m_mountPoint = path;
        LOG_INFO(QString("Mount point changed to: %1").arg(path));
    }
}

void StorageMonitor::updateRootPartition()
{
    struct statvfs stat;

    if (statvfs(m_mountPoint.toUtf8().constData(), &stat) != 0) {
        m_rootTotal = "N/A";
        m_rootUsed = "N/A";
        m_rootFree = "N/A";
        return;
    }

    // Calculate sizes in bytes
    unsigned long long total = stat.f_blocks * stat.f_frsize;
    unsigned long long available = stat.f_bavail * stat.f_frsize;
    unsigned long long used = total - available;

    // Format to human-readable strings
    m_rootTotal = formatSize(total);
    m_rootUsed = formatSize(used);
    m_rootFree = formatSize(available);
}

void StorageMonitor::updateSwap()
{
    QString content = FileReader::readAll(App::Path::PROC_MEMINFO);

    if (content.isEmpty()) {
        return;
    }

    QStringList lines = content.split('\n', Qt::SkipEmptyParts);

    unsigned long long swapTotal = 0;
    unsigned long long swapFree = 0;

    for (const QString &line: lines) {
        if (line.startsWith("SwapTotal:")) {
            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                swapTotal = parts[1].toULongLong(); // in kB
            }
        }
        else if (line.startsWith("SwapFree:")) {
            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                swapFree = parts[1].toULongLong(); // in kB
            }
        }
    }

    // Calculate swap usage
    unsigned long long swapUsed = swapTotal - swapFree;

    if (swapTotal > 0) {
        m_swapUsage = static_cast<int>((swapUsed * 100) / swapTotal);
    }
    else {
        m_swapUsage = 0;
    }

    // Convert kB to bytes for formatting
    m_swapTotal = formatSize(swapTotal * 1024);
    m_swapUsed = formatSize(swapUsed * 1024);
    m_swapFree = formatSize(swapFree * 1024);
}

void StorageMonitor::updateIoStats()
{
    if (m_blockDevice.isEmpty()) {
        return;
    }

    IoStats currentStats = readIoStats();

    // First call - initialize previous stats
    if (m_prevIoStats.sectorsRead == 0 && m_prevIoStats.sectorsWritten == 0) {
        m_prevIoStats = currentStats;
        m_ioRead = "0.0 MB/s";
        m_ioWrite = "0.0 MB/s";
        return;
    }

    // Calculate deltas
    unsigned long long deltaRead = currentStats.sectorsRead - m_prevIoStats.sectorsRead;
    unsigned long long deltaWrite = currentStats.sectorsWritten - m_prevIoStats.sectorsWritten;

    // Update previous stats
    m_prevIoStats = currentStats;

    // Calculate rates (assuming 2-second interval)
    double readRate = calculateIoRate(deltaRead);
    double writeRate = calculateIoRate(deltaWrite);
    m_ioRead = QString::number(readRate, 'f', 1) + " MB/s";
    m_ioWrite = QString::number(writeRate, 'f', 1) + " MB/s";

    // Add combined I/O to history
    int combinedIo = static_cast<int>(readRate + writeRate);
    m_ioHistory.append(combinedIo);

    // Keep only last MAX_IO_HISTORY values
    while (m_ioHistory.size() > MAX_IO_HISTORY) {
        m_ioHistory.removeFirst();
    }
}

QVariantList StorageMonitor::getIoHistory() const
{
    QVariantList result;
    result.reserve(m_ioHistory.size());

    for (int io : m_ioHistory) {
        result.append(io);
    }
    
    return result;
}

QString StorageMonitor::formatSize(unsigned long long bytes)
{
    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;
    constexpr double TB = GB * 1024.0;

    if (bytes >= TB) {
        return QString::number(bytes / TB, 'f', 1) + " TB";
    } 
    else if (bytes >= GB) {
        return QString::number(bytes / GB, 'f', 1) + " GB";
    } 
    else if (bytes >= MB) {
        return QString::number(bytes / MB, 'f', 1) + " MB";
    } 
    else if (bytes >= KB) {
        return QString::number(bytes / KB, 'f', 1) + " KB";
    }
     else {
        return QString::number(bytes) + " B";
    }
}

StorageMonitor::IoStats StorageMonitor::readIoStats()
{
    IoStats stats;

    QString statPath = QString("/sys/block/%1/stat").arg(m_blockDevice);
    QString statStr = FileReader::readFirstLine(statPath);

    if (statStr.isEmpty()) {
        return stats;
    }

    // Format: reads reads_merged sectors_read ms_reading writes writes_merged sectors_written ...
    QStringList parts = statStr.split(' ', Qt::SkipEmptyParts);

    if (parts.size() >= 7) {
        stats.sectorsRead = parts[2].toULongLong();
        stats.sectorsWritten = parts[6].toULongLong();
    }

    return stats;
}

double StorageMonitor::calculateIoRate(unsigned long long deltaSectors)
{
    // Sector size is 512 bytes
    unsigned long long bytes = deltaSectors * SECTOR_SIZE;

    // Assuming ~1 second update interval
    constexpr double interval = 1.0; // seconds
    double mbPerSecond = (bytes / (1024.0 * 1024.0)) / interval;

    return mbPerSecond;
}

QString StorageMonitor::findBlockDevice()
{
    // Priority order for block deviecs
    QStringList devices = {
#ifdef PLATFORM_RASPBERRY_PI
        "mmcblk0",      // SD card (Raspberry Pi)
#endif
        "nvme0n1",      // NVMe SSD
        "sda",          // SATA/SCSI
        "vda",          // Virtual disk (VMs)
        "xvda",         // Xen virtual disk
        "mmcblk0"       // eMMC/SD
    };

    for (const QString& device : devices) {
        QString path = QString("/sys/block/%1/stat").arg(device);
        if (FileReader::fileExists(path)) {
            return device;
        }
    }

    LOG_WARNING("No block device found for I/O monitoring");
    return QString();
}
