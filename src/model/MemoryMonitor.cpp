/**
 * ============================================
 * File: src/monitors/MemoryMonitor.cpp
 * Description: RAM monitoring implementation
 * ============================================
 */

#include "MemoryMonitor.h"
#include "FileReader.h"
#include <QDebug>

MemoryMonitor::MemoryMonitor(QObject *parent)
    : QObject(parent)
{
    qDebug() << "MemoryMonitor initialized";
}

int MemoryMonitor::parseUsage()
{
    QMap<QString, unsigned long long> memInfo = parseMemInfo();

    if (memInfo.isEmpty()) {
        return 0;
    }

    unsigned long long total = memInfo.value("MemTotal", 0);
    unsigned long long available = memInfo.value("MemAvailable", 0);

    if (total == 0) {
        return 0;
    }

    unsigned long long used = total - available;
    int usage = (used * 100) / total;

    return qBound(0, usage, 100);
}

QString MemoryMonitor::parseUsed()
{
    QMap<QString, unsigned long long> memInfo = parseMemInfo();
    
    unsigned long long total = memInfo.value("MemTotal", 0);
    unsigned long long available = memInfo.value("MemAvailable", 0);
    unsigned long long used = total - available;

    return bytesToMB(used * 1024); // meminfo is in kB
}

QString MemoryMonitor::parseFree()
{
    QMap<QString, unsigned long long> memInfo = parseMemInfo();
    unsigned long long available = memInfo.value("MemAvailable", 0);
    
    return bytesToMB(available * 1024); // meminfo is in kB
}

int MemoryMonitor::parseTotal()
{
    QMap<QString, unsigned long long> memInfo = parseMemInfo();
    unsigned long long total = memInfo.value("MemTotal", 0);
    
    return static_cast<int>(total / 1024); // Convert kB to MB
}

QString MemoryMonitor::parseCache()
{
    QMap<QString, unsigned long long> memInfo = parseMemInfo();

    // Get cached memory in kB
    unsigned long long cached = memInfo.value("Cached", 0);

    if (cached == 0) {
        qWarning() << "Failed to read Cached memory";
        return "0M";
    }
    
    // Convert kB to bytes, then to MB string
    return bytesToMB(cached * 1024);
}

QMap<QString, unsigned long long> MemoryMonitor::parseMemInfo()
{
    QMap<QString, unsigned long long> result;

    QString content = FileReader::readFile("/proc/meminfo");
    if (content.isEmpty()) {
        qWarning() << "Failed to read /proc/meminfo";
        return result;
    }

    QStringList lines = content.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QStringList parts = line.split(':', Qt::SkipEmptyParts);

        if (parts.size() < 2) {
            continue;
        }

        QString key = parts[0].trimmed();
        QString valueStr = parts[1].trimmed();

        // Remove " kB" suffix if present
        valueStr = valueStr.remove(" kB");

        bool ok;
        unsigned long long value = valueStr.toULongLong(&ok);
        
        if (ok) {
            result[key] = value;
        }
    }
    
    return result;
}

QString MemoryMonitor::bytesToMB(unsigned long long bytes)
{
    double mb = bytes / (1024.0 * 1024.0);
    return QString::number(static_cast<int>(mb)) + "M";
}
