/**
 * ============================================
 * File: src/SettingsManager.h
 * Description: Settings management, system info, logs
 * ============================================
 */

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QSettings>

class SettingsManager : public QObject 
{
    Q_OBJECT

public: 
    explicit SettingsManager(QObject * parent = nullptr);

    // ==================== SYSTEM INFORMATION ====================

    // Get system hostname
    QString getHostname() const { return m_hostname; }

    // Get OS version (from /etc/os-release)
    QString getOsVersion() const { return m_osVersion; }

    // Get kernel version
    QString getKernelVersion() const { return m_kernelVersion; }

    // Get system uptime (formatted)
    QString getupTime() const;

    // Get current system time (formatted)
    QString getSystemTime() const;

    // ==================== USER SETTINGS ====================

    // Update interval (seconds)
    int getUpdateInterval() const { return m_updateInterval; }
    void setUpdateInterval(int interval);

    // Dark mode
    bool getDarkMode() const { return m_darkMode; }
    void setDarkMode(bool enabled);

    // Sound alert
    bool getSoundAlert() const { return m_soundAlert; }
    void setSoundAlert(bool enabled);

    // ==================== WARNING THRESHOLDS ====================

    // CPU warning threshold (%)
    int getCpuWarnThreshold() const { return m_cpuWarnThreshold; }
    void setCpuWarnThreshold(int threshold);
    
    // CPU critical threshold (%)
    int getCpuCritThreshold() const { return m_cpuCritThreshold; }
    void setCpuCritThreshold(int threshold);
    
    // RAM warning threshold (%)
    int getRamWarnThreshold() const { return m_ramWarnThreshold; }
    void setRamWarnThreshold(int threshold);

    // ==================== SYSTEM LOGS ====================
    
    // Get system logs list
    QVariantList getSystemLogs() const { return m_systemLogs; }

    // Add log entry
    void addLog(const QString &level, const QString &message);

    // ==================== PERSISTENCE ====================

    // Save all settings to QSettings
    void save();

    // Load all settings from QSettings
    void load();

    // ==================== SYSTEM CONTROLS ====================
    
    // Reboot system
    void reboot();

    // Shutdown system
    void shutdown();

signals:
    // Emitted when settings change
    void settingsChanged();
    
    // Emitted when logs change
    void logsChanged();

private:
    // ==================== MEMBERS - SYSTEM INFO ====================

    QString m_hostname;
    QString m_osVersion;
    QString m_kernelVersion;

    // ==================== MEMBERS - USER SETTINGS ====================

    int m_updateInterval;
    bool m_darkMode;
    bool m_soundAlert;

    // ==================== MEMBERS - WARNING THRESHOLDS ====================

    int m_cpuWarnThreshold;
    int m_cpuCritThreshold;
    int m_ramWarnThreshold;

    // ==================== MEMBERS - LOGS ====================

    QVariantList m_systemLogs;
    static const int MAX_LOGS = 100;

    // ==================== HELPER METHODS ====================

    // Parse hostname from system
    QString parseHostname();

    // Parse OS version from /etc/os-release
    QString parseOsVersion();

    // Parse kernel version
    QString paresKerneVersion();

    // Parse uptime from /proc/uptime
    QString parseUptime() const;

    // Format uptime seconds to "Xd Xh Xm Xs"
    QString formatUptime(unsigned long long seconds) const;
};

#endif // SETTINGSMANAGER_H