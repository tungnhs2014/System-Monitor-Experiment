/**
 * ============================================
 * File: src/monitors/NetworkMonitor.cpp
 * Description: Network monitoring implementation
 * ============================================
 */

#include "NetworkMonitor.h"
#include "FileReader.h"
#include <QDebug>
#include <QDateTime>

NetworkMonitor::NetworkMonitor(QObject *parent)
    : QObject(parent)
    , m_prevRxBytes(0)
    , m_prevTxBytes(0)
    , m_prevTimestamp(0)
{
    // Auto-detect interface on construction
    m_interface = detectInterface();

    if (m_interface.isEmpty()) {
        qWarning() << "No active network interface found";
    }
    else {
        qDebug() << "Monitoring network interface: " << m_interface;
    }
}

QString NetworkMonitor::detectInterface()
{
    // Priority: wlan0 > eth0 > any other interface
    QStringList preferredInterfaces = {"wlan0", "eth0"};

    QString content = FileReader::readFile("/proc/net/dev");
    if (content.isEmpty()) {
        return QString();
    }

    QStringList lines = content.split('\n', Qt::SkipEmptyParts);
    QStringList availableInterfaces;

    // Parse available interface (skip first 2 header lines)
    for (int i = 2; i < lines.size(); i++) {
        QString line = lines[i];
        int colonPos = line.indexOf(":");

        if (colonPos > 0) {
            QString ifaceName = line.left(colonPos).trimmed();

            // Skip loopback
            if (ifaceName == "lo") {
                continue;
            }

            availableInterfaces << ifaceName;
        }
    }

    // Try preferred interfaces first
    for (const QString &preferred : preferredInterfaces) {
        if (availableInterfaces.contains(preferred)) {
            return preferred;
        }
    }

    // Return first available interface
    return availableInterfaces.isEmpty() ? QString() : availableInterfaces.first();
}

QString NetworkMonitor::parseDownloadRate()
{
    if (m_interface.isEmpty()) {
        return "0.0";
    }

    unsigned long long rxBytes, txBytes;

    if (!parseNetStats(m_interface, rxBytes, txBytes)) {
        return "0.0";
    }

    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    // First call - initialize
    if (m_prevTimestamp == 0) {
        m_prevRxBytes = rxBytes;
        m_prevTxBytes = txBytes;
        m_prevTimestamp = currentTime;
        return "0.0";
    }

    // Calculate rate
    qint64 timeDelta = currentTime - m_prevTimestamp;
    unsigned long long rxDelta = rxBytes - m_prevRxBytes;

    // Update previous values
    m_prevRxBytes = rxBytes;
    m_prevTxBytes = txBytes;
    m_prevTimestamp = currentTime;

    if (timeDelta == 0) {
        return "0.0";
    }

    // Bytes per millisecond -> Bytes per second
    double bytesPerSec = (rxDelta * 1000.0) / timeDelta;

    return formatRate(bytesPerSec);
}

QString NetworkMonitor::parseUploadRate()
{
    if (m_interface.isEmpty()) {
        return "0.0";
    }

    // Upload rate is calculated alongside download rate
    // Use stored m_prevTxBytes
    unsigned long long rxBytes, txBytes;

    if (!parseNetStats(m_interface, rxBytes, txBytes)) {
        return "0.0";
    }

    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

    if (m_prevTimestamp == 0) {
        return "0.0";
    }

    qint64 timeDelta = currentTime - m_prevTimestamp;
    unsigned long long txDelta = txBytes - m_prevTxBytes;

    if (timeDelta == 0) {
        return "0.0";
    }

    double bytesPerSec = (txDelta * 1000.0) / timeDelta;

    return formatRate(bytesPerSec);
}

QString NetworkMonitor::getInterface() const
{
    return m_interface;
}

bool NetworkMonitor::parseNetStats(const QString &interface, unsigned long long &rxBytes, unsigned long long &txBytes)
{
    QString content = FileReader::readFile("/proc/net/dev");
    if (content.isEmpty()) {
        return false;
    }

    QStringList lines = content.split('\n', Qt::SkipEmptyParts);

    // Find interface line
    for (const QString &line : lines) {
        if (!lines.contains(interface + ":")) {
            continue;
        }

        // Parse line format:
        // interface: rxBytes rxPackets ... txBytes txPackets ...
        QString data = line.split(':')[1].trimmed();
        QStringList parts = data.split(' ', Qt::SkipEmptyParts);

        if (parts.size() < 9) {
            return false;
        }
        
        bool rxOk, txOk;
        rxBytes = parts[0].toULongLong(&rxOk);
        txBytes = parts[1].toULongLong(&txOk);

        return rxOk && txOk;
    }
    
    return false;
}

QString NetworkMonitor::formatRate(double bytesPerSec)
{
    // Convert to KB/s
    double kbps = bytesPerSec / 1024.0;
    
    return QString::number(kbps, 'f', 1);
}
