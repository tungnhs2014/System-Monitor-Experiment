/**
 * ============================================
 * File: src/model/MemoryMonitor.cpp
 * Description: Memory monitoring implementation
 * ============================================
 */

#include "MemoryMonitor.h"
#include "FileReader.h"
#include "Constants.h"
#include "Logger.h"
#include <QDateTime>

MemoryMonitor::MemoryMonitor(QObject *parent)
    : QObject(parent)
    , m_lastCacheTime(0)
{
    LOG_INFO("MemoryMonitor initialized");
}

int MemoryMonitor::parseUsage()
{
    QMap<QString, unsigned long long> memInfo = parseMemInfo();

    if (memInfo.isEmpty()) {
        return 0;
    }

    unsigned long long total = memInfo.value("MemTotal", 0);
    unsigned long long available = memInfo.value("MemAvailable", 0);

    // Fallback if MemAvailable not present (older kernels)
    if (available == 0) {
         unsigned long long free = memInfo.value("MemFree", 0);
        unsigned long long buffers = memInfo.value("Buffers", 0);
        unsigned long long cached = memInfo.value("Cached", 0);
        available = free + buffers + cached;
    }

    if (total == 0) {
        return 0;
    }

    unsigned long long used = total - available;
    int usage = static_cast<int>((used * 100) / total);

    return qBound(0, usage, 100);
}

QString MemoryMonitor::parseUsed()
{
    QMap<QString, unsigned long long> memInfo = parseMemInfo();
    
    unsigned long long total = memInfo.value("MemTotal", 0);
    unsigned long long available = memInfo.value("MemAvailable", 0);

    if (available == 0) {
        unsigned long long free = memInfo.value("MemFree", 0);
        unsigned long long buffers = memInfo.value("Buffers", 0);
        unsigned long long cached = memInfo.value("Cached", 0);
        available = free + buffers + cached;
    }

    unsigned long long used = total - available;

    // meminfo values are in kB, convert to bytes for bytesToMB
    return bytesToMB(used * 1024);
}

QString MemoryMonitor::parseFree()
{
    QMap<QString, unsigned long long> memInfo = parseMemInfo();
    unsigned long long available = memInfo.value("MemAvailable", 0);

     if (available == 0) {
        unsigned long long free = memInfo.value("MemFree", 0);
        unsigned long long buffers = memInfo.value("Buffers", 0);
        unsigned long long cached = memInfo.value("Cached", 0);
        available = free + buffers + cached;
    }
    
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
    unsigned long long cached = memInfo.value("Cached", 0);

    return bytesToMB(cached * 1024);
}

QString MemoryMonitor::parseBuffers()
{
    QMap<QString, unsigned long long> memInfo = parseMemInfo();
    unsigned long long buffers = memInfo.value("Buffers", 0);
    
    return bytesToMB(buffers * 1024);
}

unsigned long long MemoryMonitor::getUsedBytes()
{
    QMap<QString, unsigned long long> memInfo = parseMemInfo();

    unsigned long long total = memInfo.value("MemTotal", 0);
    unsigned long long available = memInfo.value("MemAvailable", 0);

    if (available == 0) {
        unsigned long long free = memInfo.value("MemFree", 0);
        unsigned long long buffers = memInfo.value("Buffers", 0);
        unsigned long long cached = memInfo.value("Cached", 0);
        available = free + buffers + cached;
    }

    return (total - available) * 1024; // Convert kB to bytes
}

unsigned long long MemoryMonitor::getFreeBytes()
{
    QMap<QString, unsigned long long> memInfo = parseMemInfo();
    unsigned long long available = memInfo.value("MemAvailable", 0);
    
    if (available == 0) {
        unsigned long long free = memInfo.value("MemFree", 0);
        unsigned long long buffers = memInfo.value("Buffers", 0);
        unsigned long long cached = memInfo.value("Cached", 0);
        available = free + buffers + cached;
    }
    
    return available * 1024;
}

unsigned long long MemoryMonitor::getTotalBytes()
{
    QMap<QString, unsigned long long> memInfo = parseMemInfo();
    return memInfo.value("MemTotal", 0) * 1024;
}

unsigned long long MemoryMonitor::getCachedBytes()
{
    QMap<QString, unsigned long long> memInfo = parseMemInfo();
    return memInfo.value("Cached", 0) * 1024;
}

unsigned long long MemoryMonitor::getBuffersBytes()
{
    QMap<QString, unsigned long long> memInfo = parseMemInfo();
    return memInfo.value("Buffers", 0) * 1024;
}

QMap<QString, unsigned long long> MemoryMonitor::parseMemInfo()
{
    // Check cache validity
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    if (!m_cachedMemInfo.isEmpty() && (currentTime - m_lastCacheTime) < CACHE_VALIDITY_MS) {
        return m_cachedMemInfo;
    }

    m_cachedMemInfo.clear();

    QString content = FileReader::readAll(App::Path::PROC_MEMINFO);
    if (content.isEmpty()) {
        LOG_WARNING("Failed to read /proc/meminfo");
        return m_cachedMemInfo;
    }

    QStringList lines = content.split('\n', Qt::SkipEmptyParts);
    
    for (const QString& line: lines) {
        int colonPos = line.indexOf(':');
        if (colonPos <= 0) {
            continue;
        }

        QString key = line.left(colonPos).trimmed();
        QString valueStr = line.mid(colonPos + 1).trimmed();

        // Remove " kB" suffix
        valueStr = valueStr.remove(" kB").trimmed();

        bool ok;
        unsigned long long value = valueStr.toULongLong(&ok);

        if (ok) {
            m_cachedMemInfo[key] = value;
        }
    }
    
    m_lastCacheTime = currentTime;
    return m_cachedMemInfo;
}

QString MemoryMonitor::bytesToMB(unsigned long long bytes)
{
    double mb = bytes / (1024.0 * 1024.0);
    return QString::number(static_cast<int>(mb)) + "M";
}

QString MemoryMonitor::bytesToHuman(unsigned long long bytes)
{
    const char* units[] = {"B", "K", "M", "G", "T"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024.0 && unitIndex < 4)  {
        size /= 1024.0;
        unitIndex++;
    }

    if (unitIndex == 0) {
        return QString::number(static_cast<int>(size)) + units[unitIndex];
    }
    return QString::number(size, 'f', 1) + units[unitIndex];
}
