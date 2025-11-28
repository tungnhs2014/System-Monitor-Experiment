/**
 * ============================================
 * File: src/monitors/StorageMonitor.cpp
 * Description: Storage monitoring implementation
 * ============================================
 */

 #include "StorageMonitor.h"
 #include "FileReader.h"
 #include <QDebug>
 #include <QFile>
 #include <QTextStream>
 #include <sys/statvfs.h>

StorageMonitor::StorageMonitor(QObject *parent)
    : QObject(parent)
    , m_mountPoint("/")
    , m_rootTotal("OGB")
    , m_rootUsed("0GB")
    , m_rootFree("0GB")
    , m_swapUsage(0)
    , m_swapTotal("0MB")
    , m_swapUsed("0MB")
    , m_swapFree("0MB")
    , m_ioRead("0 MB/s")
    , m_ioWrite("0 MB/s")
{
    // Reverse space for I/O history
    m_ioHistory.reserve(MAX_IO_HISTORY);

    qDebug() << "StorageMonitor initialized - mount point:" << m_mountPoint;

#ifdef PLATFORM_RASPBERRY_PI
    qDebug() << "I/O Device: mmcblk0 (SD card)";
#else
    qDebug() << "I/O Device: sda/nvme0n1 (disk)";
#endif
}

int StorageMonitor::parseUsage()
{
    struct statvfs stat;

    if (statvfs(m_mountPoint.toUtf8().constData(), &stat) != 0) {
        static bool warned = false;
        if (!warned) {
            qWarning() << "Failed to get storage stats for" << m_mountPoint;
            warned = true;
        }
        return 0;
    }

    // Calculate usage
    unsigned long long total = stat.f_blocks * stat.f_frsize;
    unsigned long long free = stat.f_bavail * stat.f_frsize;
    unsigned long long used = total - free;

    if (total == 0) {
        return 0;
    }

    int usage = (used * 100) / total;
    return qBound(0, usage, 100);
}

int StorageMonitor::parseTemp()
{
    // SD card/disk temperature not directly available 
    return 0;
}

QString StorageMonitor::getMountPoint() const
{
    return m_mountPoint;
}

void StorageMonitor::setMountPoint(const QString &path)
{
    m_mountPoint = path;
}

void StorageMonitor::updateRootPartition()
{
    struct statvfs stat;

    if (statvfs(m_mountPoint.toUtf8().constData(), &stat) != 0) {
        m_rootTotal = "0GB";
        m_rootUsed = "0GB";
        m_rootFree = "0GB";
        return;
    }

    // Calculate sizes in bytes
    unsigned long long total = stat.f_blocks * stat.f_frsize;
    unsigned long long free = stat.f_bavail * stat.f_frsize;
    unsigned long long used = total - free;

    // Format to human-readable strings
    m_rootTotal = formatSize(total);
    m_rootUsed = formatSize(used);
    m_rootFree = formatSize(free);
}

void StorageMonitor::updateSwap()
{
    // Read /proc/meminfo for swap information
    QString content = FileReader::readFile("/proc/meminfo");

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
        m_swapUsage = (swapUsed * 100) / swapTotal;
    }
    else {
        m_swapUsage = 0;
    }

    // Format sizes (convert kB to bytes)
    m_swapTotal = formatSize(swapTotal * 1024);
    m_swapUsed = formatSize(swapUsed * 1024);
    m_swapFree = formatSize(swapFree * 1024);
}

void StorageMonitor::updateIoStats()
{
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

    // Format as strings
    m_ioRead = QString::number(readRate, 'f', 1) + "MB/s";
    m_ioWrite = QString::number(writeRate, 'f', 1) + "MB/s";

    // Add combined I/O to history (read + write inn MB/s)
    int combineIo = static_cast<int>(readRate + writeRate);
    m_ioHistory.append(combineIo);

    // Keep only last MAX_IO_HISTORY values
    if (m_ioHistory.size() > MAX_IO_HISTORY) {
        m_ioHistory.removeFirst();
    }
}

QVariantList StorageMonitor::getIoHistory() const
{
    QVariantList result;
    
    // Convert QVector<int> to QVariantList for QML
    for (int io : m_ioHistory) {
        result.append(io);
    }
    
    return result;
}

QString StorageMonitor::formatSize(unsigned long long bytes)
{
    const double KB = 1024.0;
    const double MB = KB * 1024.0;
    const double GB = MB * 1024.0;

    if (bytes >= GB) {
        double gb = bytes / GB;
        return QString::number(gb, 'f', 1) + " GB";
    }
    else if (bytes >= MB) {
        double mb = bytes / MB;
        return QString::number(mb, 'f', 1) + " MB";
    }
    else if (bytes >= KB) {
        double kb = bytes / KB;
        return QString::number(kb, 'f', 1) + " KB";
    }
    else {
        return QString::number(bytes) + " B";
    }
}

StorageMonitor::IoStats StorageMonitor::readIoStats()
{
    IoStats stats;

    // Platform-specific I/O device
#ifdef PLATFORM_RASPBERRY_PI
    // Raspberry Pi: SD card (mmcblk0)
    QString statPath = "/sys/block/mmcblk0/stat";
#else
    // Desktop/Ubuntu: Try multiple devices
    QStringList devicePaths = {
        "/sys/block/sda/stat",      // Most common (HDD/SSD)
        "/sys/block/nvme0n1/stat",  // NVMe SSD
        "/sys/block/sdb/stat",      // Alternative
        "/sys/block/vda/stat"       // Virtual disk (VM)
    };
    
    QString statPath;
    for (const QString &path : devicePaths) {
        if (FileReader::fileExits(path)) {
            statPath = path;
            static bool firstRead = true;
            if (firstRead) {
                qDebug() << "I/O stats device:"<< path;
                firstRead = false;
            }
            break;
        }
    }
    if (statPath.isEmpty()) {
        static bool warned = false;
        if (!warned) {
            qWarning() << "No I/O device found (normal on some systems)";
            warned = true;
        }
        return stats;
    }
#endif

    // Read /sys/block/*/stat/
    // Format: reads reads_merged sectors_read ms_reading writes writes_merged sectors_written ms_writing ...

    QString statStr = FileReader::readFirstLine(statPath);
    
    if (statStr.isEmpty()) {
        return stats;
    }
    
    QStringList parts = statStr.split(' ', Qt::SkipEmptyParts);

    // Index 2: sectors read
    // Index 6: sectors written
    if (parts.size() >= 7) {
        stats.sectorsRead = parts[2].toULongLong();
        stats.sectorsWritten = parts[6].toULongLong();
    }
    
    return stats;
}

double StorageMonitor::calculateIoRate(unsigned long long deltasetors)
{
    // Sector size is typically 512 bytes
    const unsigned long long sectorSize = 512;

    // Calculates bytes
    unsigned long long bytes = deltasetors * sectorSize;

    // Convert to MB/s (assuming 2-second interval)
    const double interval = 2.0; // seconds
    double mbPerSecond = (bytes / (1024.0 * 1024.0)) / interval;

    return mbPerSecond;
}
