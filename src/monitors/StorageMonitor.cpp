/**
 * ============================================
 * File: src/monitors/StorageMonitor.cpp
 * Description: Storage monitoring implementation
 * ============================================
 */

 #include "StorageMonitor.h"
 #include "FileReader.h"
 #include <QDebug>
 #include <sys/statvfs.h>

StorageMonitor::StorageMonitor(QObject *parent)
    : QObject(parent)
    , m_mountPoint("/")
{
}

int StorageMonitor::parseUsage()
{
    struct statvfs stat;

    if (statvfs(m_mountPoint.toUtf8().constData(), &stat) != 0) {
        qWarning() << "Failed to get filesytem stats for " << m_mountPoint;
        return 0;
    }

    // Calculate usage
    unsigned long long total = stat.f_blocks * stat.f_frsize;
    unsigned long long available = stat.f_bavail * stat.f_frsize;
    unsigned long long used = total - available;

    if (total == 0) {
        return 0;
    }

    int usage = (used * 100) / total;
    return qBound(0, usage, 100);
}

int StorageMonitor::parseTemp()
{
    // Try to find storage temperature sensor
    // Common paths on Raspberry Pi
    QStringList tempPaths = {
        "/sys/class/hwmon/hwmon0/temp1_input",
        "/sys/class/hwmon/hwmon1/temp1_input",
        "/sys/class/thermal/thermal_zone1/temp"
    };

    for (const QString &path : tempPaths) {
        if (!FileReader::fileExits(path)) {
            continue;
        }

        QString tempStr = FileReader::readFirstLine(path);
        bool ok;
        int tempMilliDegrees = tempStr.toInt(&ok);
        
        if (ok) {
            return tempMilliDegrees / 1000;
        }
    }

    // Fallback: Return CPU temp as approximation
    QString tempStr = FileReader::readFirstLine("/sys/class/thermal/thermal_zone0/temp");

    if (tempStr.isEmpty()) {
        return 0;
    }

    bool ok;
    int tempMilliDegrees = tempStr.toInt(&ok);

    return ok ? (tempMilliDegrees / 1000) : 0;
}

QString StorageMonitor::getMountPoint() const
{
    return m_mountPoint;
}

void StorageMonitor::setMountPoint(const QString &path)
{
    m_mountPoint = path;
}
