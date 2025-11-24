/**
 * ============================================
 * File: src/monitors/NetworkMonitor.cpp
 * Description: Network monitoring implementation
 * ============================================
 */

#include "NetworkMonitor.h"
#include "FileReader.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDateTime>

NetworkMonitor::NetworkMonitor(QObject *parent)
    : QObject(parent)
    , m_interface("")
    , m_ipAddress("N/A")
    , m_macAddress("N/A")
    , m_upSpeed("0.0 Bps")
    , m_downSpeed("0.0 Bps")
    , m_packetRate("0 pps")
    , m_activeConnections(0)
    , m_prevTimestamp(0)
{

    // Reserve space for history buffer
    m_upHistory.reserve(MAX_HISTORY);
    m_downHistory.reserve(MAX_HISTORY);


    // Detect active interface
    m_interface = detectInterface();
    
    qDebug() << "NetworkMonitor initialized - interface:" << m_interface;
    
#ifdef PLATFORM_RASPBERRY_PI
    qDebug() << "Platform: Raspberry Pi - Prefer eth0/wlan0";
#else
    qDebug() << "Platform: Desktop/Ubuntu - Prefer enp*/wlp*";
#endif
}

void NetworkMonitor::update()
{
    // Update interface if nededed (in case it changed)
    if (m_interface.isEmpty()) {
        m_interface = detectInterface();
    }

    // Update IP and MAC addresses
    m_ipAddress = parseIpAddress(m_interface);
    m_macAddress = parseMacAddress(m_interface);

    // Update trafiic statistics
    NetStats currentStats = parseNetStats(m_interface);
    qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();

    // First call - initialize previous stats
    if (m_prevTimestamp == 0) {
        m_prevStats = currentStats;
        m_prevTimestamp = currentTimestamp;
        m_upSpeed = "0.0 Bps";
        m_downSpeed = "0.0 Bps";
        m_packetRate = "0 pps";
        return;
    }

    // Calculate deltas
    qint64 deltaMs = currentTimestamp - m_prevTimestamp;
    unsigned long long deltaRxBytes = currentStats.rxBytes - m_prevStats.rxBytes;
    unsigned long long deltaTxBytes = currentStats.txBytes - m_prevStats.txBytes;
    unsigned long long deltaRxPackets = currentStats.rxPackets - m_prevStats.rxPackets;
    unsigned long long deltaTxPackets = currentStats.txPackets - m_prevStats.txPackets;
    
    // Calculate rates
    double downRate = calculateRate(deltaRxBytes, deltaMs);
    double upRate = calculateRate(deltaTxBytes, deltaMs);

    // Format speeds
    m_downSpeed = formatRate(downRate);
    m_upSpeed = formatRate(upRate);

     // Calculate packet rate (packets per second)
    if (deltaMs > 0) {
        unsigned long long totalPackets = deltaRxPackets + deltaTxPackets;
        double pps = (totalPackets * 1000.0) / deltaMs;
        m_packetRate = QString::number(static_cast<int>(pps)) + " pps";
    }
    
    // Add to history (convert to KB/s for chart)
    int upKBs = static_cast<int>(upRate / 1024.0);
    int downKBs = static_cast<int>(downRate / 1024.0);
    
    m_upHistory.append(upKBs);
    m_downHistory.append(downKBs);
    
    // Keep only last MAX_HISTORY values
    if (m_upHistory.size() > MAX_HISTORY) {
        m_upHistory.removeFirst();
    }
    if (m_downHistory.size() > MAX_HISTORY) {
        m_downHistory.removeFirst();
    }
    
    // Update previous stats
    m_prevStats = currentStats;
    m_prevTimestamp = currentTimestamp;
    
    // Update active connections
    m_activeConnections = parseActiveConnections();
}

QString NetworkMonitor::detectInterface()
{
    QStringList candidates;

    // Platform-specific interface priority
#ifdef PLATFORM_RASPBERRY_PI
    // Raspberry Pi: eth0 > wlan0
    candidates = {"eth0", "wlan0"};
#else
    // Desktop/Ubuntu: enp* > wlp* > eth0 > wlan0
    candidates = {"enp0s3", "enp0s8", "enp3s0", "ens33", 
                  "wlp2s0", "wlp3s0", "wlan0", "eth0"};
#endif

    // Try priority list first
    for (const QString &iface: candidates) {
        QString macPath = QString("/sys/class/net/%1/address").arg(iface);
        if (FileReader::fileExits(macPath)) {
            // Check if interface has carrier (is up)
            QString carrierPath = QString("/sys/class/net/%1/carrier").arg(iface);
            QString carrier = FileReader::readFirstLine(carrierPath);
            
            if (carrier == "1") {
                qDebug() << "Detected active interface:" << iface;
                return iface;
            }
        }
    }

    // Fallback: return first interface found in /proc/net/dev
    QFile devFile("/proc/net/dev");
    if (devFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&devFile);
        QString line;
        
        // Skip first 2 header lines
        in.readLine();
        in.readLine();
        
        while (!in.atEnd()) {
            line = in.readLine();
            QStringList parts = line.split(':', Qt::SkipEmptyParts);
            
            if (parts.size() >= 2) {
                QString iface = parts[0].trimmed();
                
                // Skip loopback
                if (iface != "lo") {
                    qDebug() << "Fallback interface:" << iface;
                    return iface;
                }
            }
        }
        
        devFile.close();
    }
    
    qWarning() << "No network interface detected";
    
#ifdef PLATFORM_RASPBERRY_PI
    return "eth0"; // Default fallback for Pi
#else
    return "enp0s3"; // Default fallback for Ubuntu
#endif
}

QString NetworkMonitor::parseIpAddress(const QString &interface)
{
    if (interface.isEmpty()) {
        return "N/A";
    }

    // Use 'ip addr show' command
    QProcess process;
    process.start("ip", QStringList() << "add" << "show" << interface);

    if (!process.waitForFinished(1000)) {
        qWarning() << "Failed to get IP address for" << interface;
        return "N/A";
    }

    QString output = process.readAllStandardOutput();
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    // Look for line with "inet " (IPv4)
    for (const QString &line: lines) {
        QString trimmed =  line.trimmed();
        if (trimmed.startsWith("inet ")) {
            // Format: "inet 192.168.1.100/24..."
            QStringList parts = trimmed.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                QString ipWidthMask = parts[1];
                // Remove subnet mask
                return ipWidthMask.split('/').first();
            }
        }
    }

    return "N/A";
}

QString NetworkMonitor::parseMacAddress(const QString &interface)
{
    if (interface.isEmpty()) {
        return "N/A";
    }

    QString macPath = QString("/sys/class/net/%1/address").arg(interface);
    QString mac = FileReader::readFirstLine(macPath);

    if (mac.isEmpty()) {
        return "N/A";
    }

    return mac.trimmed();
}

NetworkMonitor::NetStats NetworkMonitor::parseNetStats(const QString &interface)
{
    NetStats stats;
    
    if (interface.isEmpty()) {
        return stats;
    }
    
    QFile file("/proc/net/dev");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        static bool warned = false;
        if (!warned) {
            qWarning() << "Failed to open /proc/net/dev";
            warned = true;
        }
        return stats;
    }
    
    QTextStream in(&file);
    QString line;
    
    while (!in.atEnd()) {
        line = in.readLine();
        
        // Look for line containing the interface name
        if (line.contains(interface + ":")) {
            // Format: "interface: rxBytes rxPackets ... txBytes txPackets ..."
            QStringList parts = line.split(':', Qt::SkipEmptyParts);
            
            if (parts.size() >= 2) {
                QStringList values = parts[1].split(' ', Qt::SkipEmptyParts);
                
                // Index 0: rxBytes
                // Index 1: rxPackets
                // Index 8: txBytes
                // Index 9: txPackets
                if (values.size() >= 10) {
                    stats.rxBytes = values[0].toULongLong();
                    stats.rxPackets = values[1].toULongLong();
                    stats.txBytes = values[8].toULongLong();
                    stats.txPackets = values[9].toULongLong();
                }
            }
            
            break;
        }
    }
    
    file.close();
    return stats;
}

QVariantList NetworkMonitor::getUpHistory() const
{
    QVariantList result;
    for (int value : m_upHistory) {
        result.append(value);
    }
    return result;
}

QVariantList NetworkMonitor::getDownHistory() const
{
    QVariantList result;
    for (int value : m_downHistory) {
        result.append(value);
    }
    return result;
}

int NetworkMonitor::parseActiveConnections()
{
    QFile file("/proc/net/tcp");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return 0;
    }
    
    QTextStream in(&file);
    int count = 0;
    
    // Skip header line
    in.readLine();
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        
        // Format: "sl local_address rem_address st ..."
        // State "01" = ESTABLISHED
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        
        if (parts.size() >= 4) {
            QString state = parts[3];
            if (state == "01") {
                count++;
            }
        }
    }
    
    file.close();
    return count;
}

QString NetworkMonitor::formatRate(double bytesPerSec)
{
    const double KB = 1024.0;
    const double MB = KB * 1024.0;
    
    if (bytesPerSec >= MB) {
        double mbps = bytesPerSec / MB;
        return QString::number(mbps, 'f', 1) + " MB/s";
    }
    else if (bytesPerSec >= KB) {
        double kbps = bytesPerSec / KB;
        return QString::number(kbps, 'f', 1) + " KB/s";
    }
    else {
        return QString::number(static_cast<int>(bytesPerSec)) + " Bps";
    }
}

double NetworkMonitor::calculateRate(unsigned long long deltaBytes, qint64 deltaMs)
{
    if (deltaMs <= 0) {
        return 0.0;
    }
    
    // Convert to bytes per second
    return (deltaBytes * 1000.0) / deltaMs;
}

QString NetworkMonitor::parseDownloadRate()
{
    return m_downSpeed;
}

QString NetworkMonitor::parseUploadRate()
{
    return m_upSpeed;
}