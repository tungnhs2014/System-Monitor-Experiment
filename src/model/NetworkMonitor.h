/**
 * ============================================
 * File: src/model/NetworkMonitor.h
 * Description: Network monitoring and traffic statistics
 * ============================================
 */

#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QVariantList>

/**
 * @class NetworkMonitor
 * @brief Monitors network interface traffic, connections, and addresses
 * 
 * Reads data from:
 * - /proc/net/dev (traffic stats)
 * - /proc/net/tcp (connections)
 * - /sys/class/net (MAC address, carrier)
 * - ip command (IP address)
 */
class NetworkMonitor : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Network statistics structure
     */
    struct NetStats {
        unsigned long long rxBytes = 0;
        unsigned long long txBytes = 0;
        unsigned long long rxPackets = 0;
        unsigned long long txPackets = 0;
    };

    explicit NetworkMonitor(QObject *parent = nullptr);
    ~NetworkMonitor() override = default;

    /**
     * @brief Update all network statistics
     */    
    void update();

    // === Interface Info ===
    QString getInterface() const { return m_interface; }    
    QString getIpAddress() const { return m_ipAddress; }
    QString getMacAddress() const { return m_macAddress; }

    // === Traffic Stats ===
    QString getUpSpeed() const { return m_upSpeed; }
    QString getDownSpeed() const { return m_downSpeed; }
    QString getPacketRate() const { return m_packetRate; }
    int getActiveConnections() const { return m_activeConnections; }

    // === History for Charts ===
    QVariantList getUpHistory() const;
    QVariantList getDownHistory() const;

   // === Legacy API ===
    QString parseDownloadRate();
    QString parseUploadRate();

private:
    /**
     * @brief Detect active network interface
     */
    QString detectInterface();

    /**
     * @brief Parse IP address for interface
     */
    QString parseIpAddress(const QString& interface);

    /**
     * @brief Parse MAC address for interface
     */
    QString parseMacAddress(const QString& interface);

    /**
     * @brief Parse network statistics from /proc/net/dev
     */
    NetStats parseNetStats(const QString& interface);

    /**
     * @brief Parse active TCP connections count
     */
    int parseActiveConnections();

    /**
     * @brief Format bytes/sec to human-readable string
     */
    QString formatRate(double bytesPerSec);

    /**
     * @brief Calculate rate from delta bytes and time
     */
    double calculateRate(unsigned long long deltaBytes, qint64 deltaMs);

private:
    // Current interface name
    QString m_interface;

    // IP and MAC addresses
    QString m_ipAddress;
    QString m_macAddress;

    // Speed/rate strings
    QString m_upSpeed;
    QString m_downSpeed;

    // Packet rate
    QString m_packetRate;

    // Active connetions count
    int m_activeConnections;

    // Previous stats for delta calculation
    NetStats m_prevStats;
    qint64 m_prevTimestamp;

    // History buffers for charts
    QVector<int> m_upHistory;
    QVector<int> m_downHistory;

    // Constants
    static constexpr int MAX_HISTORY = 60;
};

#endif // NETWORKMONITOR_H