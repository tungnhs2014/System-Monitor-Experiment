/**
 * ============================================
 * File: src/monitors/MemoryMonitor.h
 * Description: RAM monitoring - usage, used/free, swap
 * ============================================
 */

#ifndef MEMORYMONITOR_H
#define MEMORYMONITOR_H

#include <QObject>
#include <QString>

class MemoryMonitor : public QObject 
{
    Q_OBJECT

public:
    explicit MemoryMonitor(QObject *parent = nullptr);

    // Parse RAM usage percentage
    int parseUsage();

    // Parse RAM used in MB
    QString parseUsed();

    // Parse RAM free in MB
    QString parseFree();

    // Parse total RAM in MB
    int parseTotal();

private:
    // Parse /proc/meminfo
    QMap<QString, unsigned long long> parseMemInfo();

    // Convert bytes to MB string
    QString bytesToMB(unsigned long long bytes);
};

#endif // MEMORYMONITOR_H