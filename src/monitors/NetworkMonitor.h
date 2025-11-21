/**
 * ============================================
 * File: src/monitors/NetworkMonitor.h
 * Description: Network monitoring - auto-detect interface, rates
 * ============================================
 */

#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include <QObject>
#include <QString>

class NetworkMonitor : public QObject
{
    Q_OBJECT

public:
    explicit NetworkMonitor(QObject *parent = nullptr);

    // Auto-detect active network interface
    QString detectInterface();

    // Parse download rate (KB/s)
    QString parseDownloadRate();

    // Parse upload rate (KB/s)
    QString parseUploadRate();

    // Get current interface name
    QString getInterface() const;

private:
    QString m_interface;

    // Previous stats for rate calculation
    unsigned long long m_prevRxBytes;
    unsigned long long m_prevTxBytes;
    qint64 m_prevTimestamp;

    // Parse /proc/net/dev
    bool parseNetStats(const QString &interface, unsigned long long &rxBytes, unsigned long long &txBytes);

    // Format bytes/sec to string
    QString formatRate(double bytesPerSec);
};

#endif // NETWORKMONITOR_H