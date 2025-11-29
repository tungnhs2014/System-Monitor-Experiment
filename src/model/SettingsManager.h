/**
 * ============================================
 * File: src/model/SettingsManager.h
 * Description: Application settings and system information management
 * ============================================================================
 */

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QSettings>

/**
 * @class SettingsManager
 * @brief Manages application settings, system info, and logging
 * 
 * Responsibilities:
 * - Save/load user settings
 * - System information (hostname, kernel, OS)
 * - Warning thresholds
 * - System logs
 * - Reboot/shutdown commands
 */
class SettingsManager : public QObject
{
    Q_OBJECT

    // Settings properties
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval NOTIFY settingsChanged)
    Q_PROPERTY(bool darkMode READ darkMode WRITE setDarkMode NOTIFY settingsChanged)
    Q_PROPERTY(bool soundAlert READ soundAlert WRITE setSoundAlert NOTIFY settingsChanged)

    // Warning thresholds
    Q_PROPERTY(int cpuWarnThreshold READ cpuWarnThreshold WRITE setCpuWarnThreshold NOTIFY settingsChanged)
    Q_PROPERTY(int cpuCritThreshold READ cpuCritThreshold WRITE setCpuCritThreshold NOTIFY settingsChanged)
    Q_PROPERTY(int ramWarnThreshold READ ramWarnThreshold WRITE setRamWarnThreshold NOTIFY settingsChanged)
    
    // System info (read-only)
    Q_PROPERTY(QString hostname READ hostname CONSTANT)
    Q_PROPERTY(QString osVersion READ osVersion CONSTANT)
    Q_PROPERTY(QString kernelVersion READ kernelVersion CONSTANT)
    Q_PROPERTY(QString uptime READ uptime NOTIFY uptimeChanged)
    Q_PROPERTY(QString systemTime READ systemTime NOTIFY systemTimeChanged)
    
    // Logs
    Q_PROPERTY(QVariantList systemLogs READ systemLogs NOTIFY logsChanged)


public: 
    explicit SettingsManager(QObject * parent = nullptr);
    ~SettingsManager() override = default;

    // === Settings Getters ===
    int updateInterval() const { return m_updateInterval; }
    bool darkMode() const { return m_darkMode; }
    bool soundAlert() const { return m_soundAlert; }
    
    // === Threshold Getters ===
    int cpuWarnThreshold() const { return m_cpuWarnThreshold; }
    int cpuCritThreshold() const { return m_cpuCritThreshold; }
    int ramWarnThreshold() const { return m_ramWarnThreshold; }

    // === System Info Getters ===
    QString hostname() const { return m_hostname; }
    QString osVersion() const { return m_osVersion; }
    QString kernelVersion() const { return m_kernelVersion; }
    QString uptime() const;
    QString systemTime() const;

    // === Logs ===
    QVariantList systemLogs() const { return m_systemLogs; }

    // === Settings Setters ===
    void setUpdateInterval(int interval);
    void setDarkMode(bool enabled);
    void setSoundAlert(bool enabled);
    void setCpuWarnThreshold(int threshold);
    void setCpuCritThreshold(int threshold);
    void setRamWarnThreshold(int threshold);

    // === Actions ===
    Q_INVOKABLE void save();
    Q_INVOKABLE void load();    
    Q_INVOKABLE void reboot();
    Q_INVOKABLE void shutdown();
    Q_INVOKABLE void addLog(const QString& level, const QString& message);
    Q_INVOKABLE void clearLogs();

signals:
    void settingsChanged();
    void logsChanged();
    void uptimeChanged();
    void systemTimeChanged();

private:
    // Parsing helpers
    QString parseHostname();
    QString parseOsVersion();
    QString parseKernelVersion();
    QString parseUptime() const;
    QString formatUptime(unsigned long long seconds) const;

private:
    // Settings
    int m_updateInterval;
    bool m_darkMode;
    bool m_soundAlert;

    // System info (cached)
    QString m_hostname;
    QString m_osVersion;
    QString m_kernelVersion;

    // Thresholds
    int m_cpuWarnThreshold;
    int m_cpuCritThreshold;
    int m_ramWarnThreshold;
    
    // Logs
    QVariantList m_systemLogs;

    // Constants
    static constexpr int MAX_LOGS = 100;
};

#endif // SETTINGSMANAGER_H