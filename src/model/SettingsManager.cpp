/**
 * ============================================
 * File: src/model/SettingsManager.cpp
 * Description: Settings management implementation
 * ============================================
 */

#include "SettingsManager.h"
#include "FileReader.h"
#include "Constants.h"
#include "Logger.h"
#include <QDateTime>
#include <QProcess>
#include <unistd.h>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_updateInterval(App::Interval::NORMAL / 1000) // Convert ms to seconds
    , m_darkMode(true)
    , m_soundAlert(false)
    , m_cpuWarnThreshold(App::Threshold::CPU_WARNING)
    , m_cpuCritThreshold(App::Threshold::CPU_CRITICAL)
    , m_ramWarnThreshold(App::Threshold::RAM_WARNING)
{
    // Parse system information
    m_hostname = parseHostname();
    m_osVersion = parseOsVersion();
    m_kernelVersion = parseKernelVersion();

    // Load saved settings
    load();

    // Add initial log
    addLog("INFO", "System Monitor started");

    LOG_INFO(QString("SettingsManager initialized - hostname: %1").arg(m_hostname));
}

QString SettingsManager::uptime() const
{
    return parseUptime();
}

QString SettingsManager::systemTime() const
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
    
    // Trim to max size
    while (m_systemLogs.size() > MAX_LOGS) {
        m_systemLogs.removeLast();
    }

    emit logsChanged();
}

void SettingsManager::clearLogs()
{
    m_systemLogs.clear();
    addLog("INFO", "Logs cleared");
}

void SettingsManager::save()
{
    QSettings settings(App::Info::ORGANIZATION, App::Info::NAME);

    // Save user settings
    settings.setValue("updateInterval", m_updateInterval);
    settings.setValue("darkMode", m_darkMode);
    settings.setValue("soundAlert", m_soundAlert);

    // Save warning thresholds
    settings.setValue("cpuWarnThreshold", m_cpuWarnThreshold);
    settings.setValue("cpuCritThreshold", m_cpuCritThreshold);
    settings.setValue("ramWarnThreshold", m_ramWarnThreshold);

    settings.sync();

    addLog("INFO", "Settings saved successfully");

    LOG_INFO("Settings saved to: " + settings.fileName());
}

void SettingsManager::load()
{
    QSettings settings(App::Info::ORGANIZATION, App::Info::NAME);

    // Load with default values;
    m_updateInterval = settings.value("updateInterval", 2).toInt();
    m_darkMode = settings.value("darkMode", true).toBool();
    m_soundAlert = settings.value("soundAlert", false).toBool();

    m_cpuWarnThreshold = settings.value("cpuWarnThreshold", App::Threshold::CPU_WARNING).toInt();
    m_cpuCritThreshold = settings.value("cpuCritThreshold", App::Threshold::CPU_CRITICAL).toInt();
    m_ramWarnThreshold = settings.value("ramWarnThreshold", App::Threshold::RAM_WARNING).toInt();
    
    LOG_INFO("Settings loaded from: " + settings.fileName());
}

void SettingsManager::reboot()
{
    addLog("WARN", "System reboot initiated");
    LOG_WARNING("System reboot initiated by user");

    // Execute reboot command directly
    QProcess::startDetached("reboot", QStringList());
}   

void SettingsManager::shutdown()
{
    addLog("WARN", "System shutdown initiated");
    LOG_WARNING("System shutdown initiated by user");

    // Execute shutdown command directly
    QProcess::startDetached("shutdown", {"-h", "now"});
}

QString SettingsManager::parseHostname()
{
    char hostname[256];

    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return QString::fromUtf8(hostname);
    }

    // Fallback: read from file
    QString hostnameFile = FileReader::readFirstLine(App::Path::ETC_HOSTNAME);
    if (!hostnameFile.isEmpty()) {
        return hostnameFile;
    }

    return "unknown";
}

QString SettingsManager::parseOsVersion()
{
    QString content = FileReader::readAll(App::Path::ETC_OS_RELEASE);

    if (content.isEmpty()) {
        return "Unknown OS";
    }

    QStringList lines = content.split('\n', Qt::SkipEmptyParts);

    // Look for PRETTY_NAME first
    for (const QString &line : lines) {
        if (line.startsWith("PRETTY_NAME=")) {
            QString prettyName = line.mid(12); // Remove "PRETTY_NAME="
            // Remove quotes
            prettyName = prettyName.remove('"');
            return prettyName;
        }
    }

     // Fallback to NAME + VERSION
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
        return version.isEmpty() ? name : name + " " + version;
    }
    
    return "Linux";
}

QString SettingsManager::parseKernelVersion()
{
    // Execute 'uname -r' to get kernel version
    QProcess process;
    process.start("uname", {"-r"});

    if (!process.waitForFinished(1000)) {
        return "Unknown";
    }
    
    QString version = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    return version.isEmpty() ? "Unknown" : version;
}

QString SettingsManager::parseUptime() const
{
    // Read /proc/uptime
    QString uptimeStr = FileReader::readFirstLine(App::Path::PROC_UPTIME);

    if (uptimeStr.isEmpty()) {
        return "N/A";
    }

    // Format: "12345.67 12345.67"
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
    constexpr unsigned long long MINUTE = 60;
    constexpr unsigned long long HOUR = MINUTE * 60;
    constexpr unsigned long long DAY = HOUR * 24;

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