/**
 * ============================================
 * File: src/model/NetworkMonitor.cpp
 * Description: Network monitoring implementation
 * ============================================
 */

#include "NetworkMonitor.h"
#include "FileReader.h"
#include "Constants.h"
#include "Logger.h"
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDateTime>

NetworkMonitor::NetworkMonitor(QObject *parent)
    : QObject(parent)
    , m_interface("")
    , m_ipAddress("N/A")
    , m_macAddress("N/A")
    , m_upSpeed("0 B/s")
    , m_downSpeed("0 B/s")
    , m_packetRate("0 pps")
    , m_activeConnections(0)
    , m_prevTimestamp(0)
{
    m_upHistory.reserve(MAX_HISTORY);
    m_downHistory.reserve(MAX_HISTORY);

    // Detect active interface
    m_interface = detectInterface();
    
    LOG_INFO(QString("NetworkMonitor initialized - interface: %1").arg(m_interface));
}

void NetworkMonitor::update()
{
     // Re-detect interface if needed
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
        m_upSpeed = "0 B/s";
        m_downSpeed = "0 B/s";
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
    while (m_upHistory.size() > MAX_HISTORY) {
        m_upHistory.removeFirst();
    }
    while (m_downHistory.size() > MAX_HISTORY) {
        m_downHistory.removeFirst();
    }
    
    // Update previous values
    m_prevStats = currentStats;
    m_prevTimestamp = currentTimestamp;
    
    // Update active connections
    m_activeConnections = parseActiveConnections();
}

QString NetworkMonitor::detectInterface()
{
    QStringList candidates;

#ifdef PLATFORM_RASPBERRY_PI
    // Raspberry Pi: eth0 > wlan0
    candidates = {"eth0", "wlan0"};
#else
    candidates = {"enp0s3", "enp0s8", "enp3s0", "ens33", "ens160",
                  "wlp2s0", "wlp3s0", "wlan0", "eth0"};
#endif

    // Try priority list first
    for (const QString &iface: candidates) {
        QString macPath = QString("/sys/class/net/%1/address").arg(iface);
        if (FileReader::fileExists(macPath)) {
            // Check if interface has carrier (is up)
            QString carrierPath = QString("/sys/class/net/%1/carrier").arg(iface);
            QString carrier = FileReader::readFirstLine(carrierPath);
            
            if (carrier == "1") {
                return iface;
            }
        }
    }

    // Fallback: scan /proc/net/dev
    QStringList lines = FileReader::readLines(App::Path::PROC_NET_DEV);

    for (int i = 2; i < lines.size(); i++) { // Skip 2 header lines
        QString line = lines[i];
        int colonPos = line.indexOf(':');

        if (colonPos > 0) {
            QString iface = line.left(colonPos).trimmed();

            if (iface != "lo") {
                return iface;
            }
        }
    }

    LOG_WARNING("No network interface detected");    
    
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
    process.start("ip", {"addr", "show", interface});

    if (!process.waitForFinished(1000)) {
        return "N/A";
    }

    QString output = QString::fromUtf8(process.readAllStandardOutput());
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    // Look for line with "inet " (IPv4)
    for (const QString &line: lines) {
        QString trimmed =  line.trimmed();
        if (trimmed.startsWith("inet ")) {
            // Format: "inet 192.168.1.100/24..."
            QStringList parts = trimmed.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                // Remove subnet mask (/24)
                return parts[1].split('/').first();
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

    return mac.isEmpty() ? "N/A" : mac.toUpper();
}

NetworkMonitor::NetStats NetworkMonitor::parseNetStats(const QString &interface)
{
    NetStats stats;
    
    if (interface.isEmpty()) {
        return stats;
    }
    
    QStringList lines = FileReader::readLines(App::Path::PROC_NET_DEV);

    for (const QString& line : lines) {
        if (line.contains(interface + ":")) {
            int colonPos = line.indexOf(':');

            if (colonPos > 0) {
                QString values = line.mid(colonPos + 1);
                QStringList parts = values.split(' ', Qt::SkipEmptyParts);

                // Index: 0=rxBytes, 1=rxPackets, 8=txBytes, 9=txPackets
                if (parts.size() >= 10) {
                    stats.rxBytes = parts[0].toULongLong();
                    stats.rxPackets = parts[1].toULongLong();
                    stats.txBytes = parts[8].toULongLong();
                    stats.txPackets = parts[9].toULongLong();
                }
            }
            break;
        }
    }
    
    return stats;
}

QVariantList NetworkMonitor::getUpHistory() const
{
    QVariantList result;
    result.reserve(m_upHistory.size());

    for (int value : m_upHistory) {
        result.append(value);
    }
    return result;
}

QVariantList NetworkMonitor::getDownHistory() const
{
    QVariantList result;
    result.reserve(m_downHistory.size());

    for (int value : m_downHistory) {
        result.append(value);
    }
    return result;
}

int NetworkMonitor::parseActiveConnections()
{
    int count = 0;

    // Count IPv4 connections
    QStringList tcpLines = FileReader::readLines(App::Path::PROC_NET_TCP);
    for (int i = 1; i < tcpLines.size(); i++) { // Skip header
        QStringList parts = tcpLines[i].split(' ', Qt::SkipEmptyParts);

        if (parts.size() >= 4 && parts[3] == "01") { // ESTABLISHED
            count++;
        }
    }

    // Count IPv6 connections
     QStringList tcp6Lines = FileReader::readLines(App::Path::PROC_NET_TCP6);
    for (int i = 1; i < tcp6Lines.size(); i++) { 
        QStringList parts = tcp6Lines[i].split(' ', Qt::SkipEmptyParts);

        if (parts.size() >= 4 && parts[3] == "01") { 
            count++;
        }
    }
    
    return count;
}

QString NetworkMonitor::formatRate(double bytesPerSec)
{
    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;
    
    if (bytesPerSec >= GB) {
        return QString::number(bytesPerSec / GB, 'f', 1) + " GB/s";
    } 
    else if (bytesPerSec >= MB) {
        return QString::number(bytesPerSec / MB, 'f', 1) + " MB/s";
    } 
    else if (bytesPerSec >= KB) {
        return QString::number(bytesPerSec / KB, 'f', 1) + " KB/s";
    } 
    else {
        return QString::number(static_cast<int>(bytesPerSec)) + " B/s";
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