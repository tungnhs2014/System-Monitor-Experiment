/**
 * ============================================
 * File: src/SettingsManager.cpp
 * Description: Settings management implementation
 * ============================================
 */

#include "SettingsManager.h"
#include "FileReader.h"
#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <unistd.h>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_updateInterval(2)
    , m_darkMode(true)
    , m_soundAlert(false)
    , m_cpuWarnThreshold(70)
    , m_cpuCritThreshold(90)
    , m_ramWarnThreshold(80)
{
    // Parse system information
    m_hostname = parseHostname();
    m_osVersion = parseOsVersion();
    m_kernelVersion = paresKerneVersion();

    // Load saved settings
    load();

    // Add initial log
    addLog("INFO", "System Monitor started");

    qDebug() << "SettingManager initialized - hostname:" << m_hostname;
}

QString SettingsManager::getupTime() const
{
    return parseUptime();
}

QString SettingsManager::getSystemTime() const
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

void SettingsManager::setUpdateInterval(int interval)
{
    if (m_updateInterval != interval && interval >= 1 && interval <= 10) {
        m_updateInterval = interval;
        addLog("INFO", QString("Update interval changed to %1s").arg(interval));
        emit settingsChanged();
    }
}

void SettingsManager::setDarkMode(bool enabled)
{
    if (m_darkMode != enabled) {
        m_darkMode = enabled;
        addLog("INFO", QString("Dark mode %1").arg(enabled ? "enabled" : "disabled"));
        emit settingsChanged();
    }
}

void SettingsManager::setSoundAlert(bool enabled)
{
    if (m_soundAlert != enabled) {
        m_soundAlert = enabled;
        addLog("INFO", QString("Sound alert %1").arg(enabled ? "enabled" : "disabled"));
        emit settingsChanged();
    }
}

void SettingsManager::setCpuWarnThreshold(int threshold)
{
    if (m_cpuWarnThreshold != threshold && threshold >= 0 && threshold <= 100) {
        m_cpuWarnThreshold = threshold;
        addLog("INFO", QString("CPU warning threshold set to %1%").arg(threshold));
        emit settingsChanged();
    }
}

void SettingsManager::setCpuCritThreshold(int threshold)
{
    if (m_cpuCritThreshold != threshold && threshold >= 0 && threshold <= 100) {
        m_cpuCritThreshold = threshold;
        addLog("INFO", QString("CPU critical threshold set to %1%").arg(threshold));
        emit settingsChanged();
    }
}

void SettingsManager::setRamWarnThreshold(int threshold)
{
    if (m_ramWarnThreshold != threshold && threshold >= 0 && threshold <= 100) {
        m_ramWarnThreshold = threshold;
        addLog("INFO", QString("RAM warning threshold set to %1%").arg(threshold));
        emit settingsChanged();
    }
}

void SettingsManager::addLog(const QString &level, const QString &message)
{
    QVariantMap log;
    log["time"] = QTime::currentTime().toString("hh:mm:ss");
    log["level"] = level;
    log["message"] = message;
    
    // Add to front of list (most recent first)
    m_systemLogs.prepend(log);
    
    // Keep only last MAX_LOGS entries
    if (m_systemLogs.size() > MAX_LOGS) {
        m_systemLogs.removeLast();
    }

    emit logsChanged();
}

void SettingsManager::save()
{
    QSettings settings("ILI9341", "SystemMonitor");

    // Save user settings
    settings.setValue("updateInterval", m_updateInterval);
    settings.setValue("darkMode", m_darkMode);
    settings.setValue("soundAlert", m_soundAlert);

    // Save warning thresholds
    settings.setValue("cpuWarnThreshold", m_cpuWarnThreshold);
    settings.setValue("cpuCritThreshold", m_cpuCritThreshold);
    settings.setValue("ramWarnThreshold", m_ramWarnThreshold);

    settings.sync();

    addLog("INFO", "Setting saved sucessfully");

    qDebug() << "Settings saved to:" << settings.fileName();
}

void SettingsManager::load()
{
    QSettings settings("ILI9341", "SystemMonitor");

    // Load with default values;
    m_updateInterval = settings.value("updateInterval", 2).toInt();
    m_darkMode = settings.value("darkMode", true).toBool();
    m_soundAlert = settings.value("soundAlert", false).toBool();

    m_cpuWarnThreshold = settings.value("cpuWarnThreshold", 70).toInt();
    m_cpuCritThreshold = settings.value("cpuCritThreshold", 90).toInt();
    m_ramWarnThreshold = settings.value("ramWarnThreshold", 80).toInt();

    qDebug() << "Settings loaded from:" << settings.fileName();
}

void SettingsManager::reboot()
{
    addLog("WARN", "System reboot initialized");

    // Execute reboot command with sudo
    QProcess::execute("sudo", QStringList() << "reboot");
}   

void SettingsManager::shutdown()
{
    addLog("WARN", "System shutdown initialized");

    // Execute shutdown command with sudo
    QProcess::execute("sudo", QStringList() << "shshutdown" << "-h" <<"now");
}

QString SettingsManager::parseHostname()
{
    char hostname[256];

    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return QString::fromUtf8(hostname);
    }

    qWarning() << "Failed to get hostname";
    return "unknown";
}

QString SettingsManager::parseOsVersion()
{
    // Read /etc/os-release
    QString content = FileReader::readFile("/etc/os-release");

    if (content.isEmpty()) {
        return "Unknown OS";
    }

    QStringList lines = content.split('\n', Qt::SkipEmptyParts);

    // Look for PRETTY_NAME line
    for (const QString &line : lines) {
        if (line.startsWith("PRETTY_NAME=")) {
            QString prettyName = line.mid(12); // Remove "PRETTY_NAME="
            // Remove quotes
            prettyName = prettyName.remove('"');
            return prettyName;
        }
    }

     // Fallback: look for NAME and VERSION
    QString name, version;
    
    for (const QString &line : lines) {
        if (line.startsWith("NAME=")) {
            name = line.mid(5).remove('"');
        }
        else if (line.startsWith("VERSION=")) {
            version = line.mid(8).remove('"');
        }
    }
    
    if (!name.isEmpty()) {
        if (!version.isEmpty()) {
            return name + " " + version;
        }
        return name;
    }
    
    return "Linux";
}

QString SettingsManager::paresKerneVersion()
{
    // Execute 'uname -r' to get kernel version
    QProcess process;
    process.start("uname", QStringList() <<"-r");

    if (!process.waitForFinished(1000)) {
        qWarning() << "Failed to get kernel version";
        return "Unknown";
    }

    QString version = process.readAllStandardOutput().trimmed();

    if (version.isEmpty()) {
        return "Unknown";
    }

    return version;
}

QString SettingsManager::parseUptime() const
{
    // Read /proc/uptime
    QString uptimeStr = FileReader::readFirstLine("/proc/uptimes");

    if (uptimeStr.isEmpty()) {
        return "N/A";
    }

    // Format: "12345.67 12345.67"
    // First number is total uptime in seconds
    QStringList parts = uptimeStr.split(' ', Qt::SkipEmptyParts);

    if (parts.isEmpty()) {
        return "N/A";
    }

    bool ok;
    double uptimeSeconds = parts[0].toDouble(&ok);

    if (!ok) {
        return "N/A";
    }

    return formatUptime(static_cast<unsigned long long>(uptimeSeconds));
}

QString SettingsManager::formatUptime(unsigned long long seconds) const
{
    const unsigned long long MINUTE = 60;
    const unsigned long long HOUR = MINUTE * 60;
    const unsigned long long DAY = HOUR * 24;

    unsigned long long days = seconds / DAY;
    seconds %= DAY;

    unsigned long long hours = seconds / HOUR;
    seconds %= HOUR;
    
    unsigned long long minutes = seconds / MINUTE;
    seconds %= MINUTE;
    
    QString result;
    
    if (days > 0) {
        result += QString::number(days) + "d ";
    }
    if (hours > 0 || days > 0) {
        result += QString::number(hours) + "h ";
    }
    if (minutes > 0 || hours > 0 || days > 0) {
        result += QString::number(minutes) + "m ";
    }

    result += QString::number(seconds) + "s";
    
    return result.trimmed();
}
