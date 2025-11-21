/**
 * ============================================
 * File: src/monitors/StorageMonitor.h
 * Description: Storage monitoring - disk usage, temperature
 * ============================================
 */

#ifndef STORAGEMONITOR_H
#define STORAGEMONITOR_H

#include <QObject>
#include <QString>

class StorageMonitor : public QObject 
{
    Q_OBJECT

public:
    explicit StorageMonitor(QObject *parent = nullptr);

    // Parse storage usage percentage
    int parseUsage();

    // Parse storage temperature
    int parseTemp();

    // Get mount point to monitor (default: /)
    QString getMountPoint() const;
    void setMountPoint(const QString &path);

private:
    QString m_mountPoint;
};

#endif // STORAGEMONITOR_H