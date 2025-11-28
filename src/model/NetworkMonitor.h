/**
 * ============================================
 * File: src/monitors/NetworkMonitor.h
 * Description: Network monitoring - interface detection, traffic, connections
 * ============================================
 */

#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QVariantList>

class NetworkMonitor : public QObject
{
    Q_OBJECT

public:
    explicit NetworkMonitor(QObject *parent = nullptr);

    // ==================== MAIN UPDATE ====================

    // Update all network statistics (call this every 2 seconds)
    void update();

    // ==================== INTERFACE DETECTION ===================
    
    // Auto-detect active network interface
    QString detectInterface();

    // Get current interface name
    QString getInterface() const { return m_interface; }

    // ==================== IP/MAC ADDRESS ====================
    
    // Get IP address of current interface
    QString getIpAddress() const { return m_ipAddress; }

    // Get MAC address of current interface
    QString getMacAddress() const { return m_macAddress; }

    // ==================== TRAFFIC RATES ====================

    // Get current upload speed (formatted string)
    QString getUpSpeed() const { return m_upSpeed; }

    // Get current download speed (formatted string)
    QString getDownSpeed() const { return m_downSpeed; }

    // ==================== TRAFFIC HISTORY ====================

    // Get upload history (last 30 values)
    QVariantList getUpHistory() const;

    // Get download history (last 30 values)
    QVariantList getDownHistory() const;

    // ==================== CONNECTION STATISTICS ====================

    // Get packet rate (packet per second)
    QString getPacketRate() const { return m_packetRate; }

    // Get number of active TCP connections
    int getActiveConnections() const { return m_activeConnections; }

    // Parse download rate (KB/s)
    QString parseDownloadRate();

    // Parse upload rate (KB/s)
    QString parseUploadRate();

private:
    // ==================== STRUCTURES ====================

    // Network statistics from /proc/net/dev
    struct NetStats {
        unsigned long long rxBytes;
        unsigned long long txBytes;
        unsigned long long rxPackets;
        unsigned long long txPackets;
        
        NetStats() : rxBytes(0), txBytes(0), rxPackets(0), txPackets(0) {}
    };

    // Current interface name
    QString m_interface;

    // IP and MAC addresses
    QString m_ipAddress;
    QString m_macAddress;

    // Current speed
    QString m_upSpeed;
    QString m_downSpeed;

    // Packet rate
    QString m_packetRate;

    // Active connetions count
    int m_activeConnections;

    // Previous stats for delta calculation
    NetStats m_prevStats;
    qint64 m_prevTimestamp;

    // Traffic history buffer (circular, max 30 values = 60 seconds)
    QVector<int> m_upHistory;       // Upload in KB/s
    QVector<int> m_downHistory;  // Download innn KB/s
    static const int MAX_HISTORY = 30;

    // ==================== HELPER METHODS ====================

    // Parse IP addess for given interface
    QString parseIpAddress(const QString &interface);

    // Parse MAC address for given interface
    QString parseMacAddress(const QString &interface);

    // Parse network statistics from /proc/net/dev
    NetStats parseNetStats(const QString &interface);

    //Count active TCP connections to from /proc/net/tcp
    int parseActiveConnections();

    // Format rate in bytes/sec to human-readable string
    QString formatRate(double bytesPerSec);

    // Calculate rate from delta bytes and delta time
    double calculateRate(unsigned long long deltaBytes, qint64 deltaMs);
};

#endif // NETWORKMONITOR_H