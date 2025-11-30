/**
 * ============================================================================
 * File: src/controller/SystemController.h
 * Description: Main controller - aggregates all monitors and exposes to QML
 * ============================================================================
 */

#ifndef SYSTEMCONTROLLER_H
#define SYSTEMCONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <QVariantList>

// Forward declarations
class CpuMonitor;
class GpuMonitor;
class MemoryMonitor;
class StorageMonitor;
class NetworkMonitor;
class SettingsManager;


/**
 * @class SystemController
 * @brief Central controller that coordinates all monitors and settings
 * 
 * Responsibilities:
 * - Create and manage monitor instances
 * - Periodic update of all metrics
 * - Expose data to QML via Q_PROPERTY
 * - Handle user actions (settings, reboot, shutdown)
 */
class SystemController : public QObject
{
    Q_OBJECT

    // ==================== CPU Properties ====================
    Q_PROPERTY(int cpuUsage READ cpuUsage NOTIFY cpuUsageChanged)
    Q_PROPERTY(int cpuTemp READ cpuTemp NOTIFY cpuTempChanged)
    Q_PROPERTY(QString cpuClock READ cpuClock NOTIFY cpuClockChanged)
    Q_PROPERTY(QVariantList coreUsages READ coreUsages NOTIFY coreUsagesChanged)
    Q_PROPERTY(QVariantList tempHistory READ tempHistory NOTIFY tempHistoryChanged)
    Q_PROPERTY(QString loadAverage READ loadAverage NOTIFY loadAverageChanged)

    // ==================== GPU Properties ====================
    Q_PROPERTY(int gpuUsage READ gpuUsage NOTIFY gpuUsageChanged)
    Q_PROPERTY(int gpuTemp READ gpuTemp NOTIFY gpuTempChanged)
    Q_PROPERTY(int gpuMemUsage READ gpuMemUsage NOTIFY gpuMemUsageChanged)

    // ==================== RAM Properties ====================
    Q_PROPERTY(int ramUsage READ ramUsage NOTIFY ramUsageChanged)
    Q_PROPERTY(QString ramUsed READ ramUsed NOTIFY ramUsedChanged)
    Q_PROPERTY(QString ramFree READ ramFree NOTIFY ramFreeChanged)
    Q_PROPERTY(QString ramCache READ ramCache NOTIFY ramCacheChanged)
    Q_PROPERTY(int ramTotal READ ramTotal NOTIFY ramTotalChanged)

    // ==================== Storage Properties ====================
    Q_PROPERTY(int hddUsage READ hddUsage NOTIFY hddUsageChanged)
    Q_PROPERTY(int hddTemp READ hddTemp NOTIFY hddTempChanged)
    Q_PROPERTY(QString hddTotal READ hddTotal NOTIFY hddTotalChanged)
    Q_PROPERTY(QString hddUsed READ hddUsed NOTIFY hddUsedChanged)
    Q_PROPERTY(QString hddFree READ hddFree NOTIFY hddFreeChanged)

    // Swap
    Q_PROPERTY(int swapUsage READ swapUsage NOTIFY swapUsageChanged)
    Q_PROPERTY(QString swapTotal READ swapTotal NOTIFY swapTotalChanged)
    Q_PROPERTY(QString swapUsed READ swapUsed NOTIFY swapUsedChanged)
    Q_PROPERTY(QString swapFree READ swapFree NOTIFY swapFreeChanged)
    
    // I/O
    Q_PROPERTY(QString ioRead READ ioRead NOTIFY ioReadChanged)
    Q_PROPERTY(QString ioWrite READ ioWrite NOTIFY ioWriteChanged)
    Q_PROPERTY(QVariantList ioHistory READ ioHistory NOTIFY ioHistoryChanged)

    // ==================== Network Properties ====================
    Q_PROPERTY(QString networkInterface READ networkInterface NOTIFY networkInterfaceChanged)
    Q_PROPERTY(QString ipAddress READ ipAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(QString macAddress READ macAddress NOTIFY macAddressChanged)
    Q_PROPERTY(QString netUpSpeed READ netUpSpeed NOTIFY netUpSpeedChanged)
    Q_PROPERTY(QString netDownSpeed READ netDownSpeed NOTIFY netDownSpeedChanged)
    Q_PROPERTY(QString packetRate READ packetRate NOTIFY packetRateChanged)
    Q_PROPERTY(int activeConnections READ activeConnections NOTIFY activeConnectionsChanged)
    Q_PROPERTY(QVariantList netUpHistory READ netUpHistory NOTIFY netUpHistoryChanged)
    Q_PROPERTY(QVariantList netDownHistory READ netDownHistory NOTIFY netDownHistoryChanged)
  
    // ==================== System Properties ====================
    Q_PROPERTY(QString hostname READ hostname CONSTANT)
    Q_PROPERTY(QString osVersion READ osVersion CONSTANT)
    Q_PROPERTY(QString kernelVersion READ kernelVersion CONSTANT)
    Q_PROPERTY(QString uptime READ uptime NOTIFY uptimeChanged)
    Q_PROPERTY(QString systemTime READ systemTime NOTIFY systemTimeChanged)
    
    // ==================== Settings Properties ====================
    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval NOTIFY updateIntervalChanged)
    Q_PROPERTY(bool darkMode READ darkMode WRITE setDarkMode NOTIFY darkModeChanged)
    Q_PROPERTY(bool soundAlert READ soundAlert WRITE setSoundAlert NOTIFY soundAlertChanged)
    
    Q_PROPERTY(int cpuWarnThreshold READ cpuWarnThreshold WRITE setCpuWarnThreshold NOTIFY cpuWarnThresholdChanged)
    Q_PROPERTY(int cpuCritThreshold READ cpuCritThreshold WRITE setCpuCritThreshold NOTIFY cpuCritThresholdChanged)
    Q_PROPERTY(int ramWarnThreshold READ ramWarnThreshold WRITE setRamWarnThreshold NOTIFY ramWarnThresholdChanged)
    
    // ==================== Logs ====================
    Q_PROPERTY(QVariantList systemLogs READ systemLogs NOTIFY systemLogsChanged)

public:
    explicit SystemController(QObject* parent = nullptr);
    ~SystemController() override;

    // ==================== CPU Getters ====================
    int cpuUsage() const { return m_cpuUsage; }
    int cpuTemp() const { return m_cpuTemp; }
    QString cpuClock() const { return m_cpuClock; }
    QVariantList coreUsages() const { return m_coreUsages; }
    QVariantList tempHistory() const { return m_tempHistory; }
    QString loadAverage() const { return m_loadAverage; }

    // ==================== GPU Getters ====================
    int gpuUsage() const { return m_gpuUsage; }
    int gpuTemp() const { return m_gpuTemp; }
    int gpuMemUsage() const { return m_gpuMemUsage; }

    // ==================== RAM Getters ====================
    int ramUsage() const { return m_ramUsage; }
    QString ramUsed() const { return m_ramUsed; }
    QString ramFree() const { return m_ramFree; }
    QString ramCache() const { return m_ramCache; }
    int ramTotal() const { return m_ramTotal; }

    // ==================== Storage Getters ====================
    int hddUsage() const { return m_hddUsage; }
    int hddTemp() const { return m_hddTemp; }
    QString hddTotal() const { return m_hddTotal; }
    QString hddUsed() const { return m_hddUsed; }
    QString hddFree() const { return m_hddFree; }
    int swapUsage() const { return m_swapUsage; }
    QString swapTotal() const { return m_swapTotal; }
    QString swapUsed() const { return m_swapUsed; }
    QString swapFree() const { return m_swapFree; }
    QString ioRead() const { return m_ioRead; }
    QString ioWrite() const { return m_ioWrite; }
    QVariantList ioHistory() const { return m_ioHistory; }

    // ==================== Network Getters ====================
    QString networkInterface() const { return m_networkInterface; }
    QString ipAddress() const { return m_ipAddress; }
    QString macAddress() const { return m_macAddress; }
    QString netUpSpeed() const { return m_netUpSpeed; }
    QString netDownSpeed() const { return m_netDownSpeed; }
    QString packetRate() const { return m_packetRate; }
    int activeConnections() const { return m_activeConnections; }
    QVariantList netUpHistory() const { return m_netUpHistory; }
    QVariantList netDownHistory() const { return m_netDownHistory; }

    // ==================== System Getters ====================
    QString hostname() const { return m_hostname; }
    QString osVersion() const { return m_osVersion; }
    QString kernelVersion() const { return m_kernelVersion; }
    QString uptime() const { return m_uptime; }
    QString systemTime() const { return m_systemTime; }

    // ==================== Settings Getters ====================
    int updateInterval() const { return m_updateInterval; }
    bool darkMode() const { return m_darkMode; }
    bool soundAlert() const { return m_soundAlert; }
    int cpuWarnThreshold() const { return m_cpuWarnThreshold; }
    int cpuCritThreshold() const { return m_cpuCritThreshold; }
    int ramWarnThreshold() const { return m_ramWarnThreshold; }

    // ==================== Logs ====================
    QVariantList systemLogs() const { return m_systemLogs; }

    // ==================== Settings Setters ====================
    void setUpdateInterval(int interval);
    void setDarkMode(bool enabled);
    void setSoundAlert(bool enabled);
    void setCpuWarnThreshold(int threshold);
    void setCpuCritThreshold(int threshold);
    void setRamWarnThreshold(int threshold);

    // ==================== Invokable Methods ====================
    Q_INVOKABLE void saveSettings();
    Q_INVOKABLE void reboot();
    Q_INVOKABLE void shutdown();
    Q_INVOKABLE void addLog(const QString& level, const QString& message);

signals:
    // CPU Signals
    void cpuUsageChanged();
    void cpuTempChanged();
    void cpuClockChanged();
    void coreUsagesChanged();
    void tempHistoryChanged();
    void loadAverageChanged();

    // GPU Signals
    void gpuUsageChanged();
    void gpuTempChanged();
    void gpuMemUsageChanged();

    // RAM Signals
    void ramUsageChanged();
    void ramUsedChanged();
    void ramFreeChanged();
    void ramCacheChanged();
    void ramTotalChanged();

    // Storage Signals
    void hddUsageChanged();
    void hddTempChanged();
    void hddTotalChanged();
    void hddUsedChanged();
    void hddFreeChanged();
    void swapUsageChanged();
    void swapTotalChanged();
    void swapUsedChanged();
    void swapFreeChanged();
    void ioReadChanged();
    void ioWriteChanged();
    void ioHistoryChanged();

    // Network Signals
    void networkInterfaceChanged();
    void ipAddressChanged();
    void macAddressChanged();
    void netUpSpeedChanged();
    void netDownSpeedChanged();
    void packetRateChanged();
    void activeConnectionsChanged();
    void netUpHistoryChanged();
    void netDownHistoryChanged();

    // System Signals
    void uptimeChanged();
    void systemTimeChanged();

    // Settings
    void updateIntervalChanged();
    void darkModeChanged();
    void soundAlertChanged();
    void cpuWarnThresholdChanged();
    void cpuCritThresholdChanged();
    void ramWarnThresholdChanged();

    // Logs
    void systemLogsChanged();
    
private slots:
    void updateAllMetrics();

private:
    void initializeMonitors();
    void connectSignals();
    void checkThresholds();

private:
    // Monitors (Model layer)
    CpuMonitor* m_cpuMonitor;
    GpuMonitor* m_gpuMonitor;
    MemoryMonitor* m_memMonitor;
    StorageMonitor* m_storageMonitor;
    NetworkMonitor* m_networkMonitor;
    SettingsManager* m_settingsManager;

    // Update timer
    QTimer* m_updateTimer;

    // CPU data
    int m_cpuUsage;
    int m_cpuTemp;
    QString m_cpuClock;
    QVariantList m_coreUsages;
    QVariantList m_tempHistory;
    QString m_loadAverage;

    // GPU data
    int m_gpuUsage;
    int m_gpuTemp;
    int m_gpuMemUsage;

    // RAM data
    int m_ramUsage;
    QString m_ramUsed;
    QString m_ramFree;
    QString m_ramCache;
    int m_ramTotal;

    // Storage data
    int m_hddUsage;
    int m_hddTemp;
    QString m_hddTotal;
    QString m_hddUsed;
    QString m_hddFree;
    int m_swapUsage;
    QString m_swapTotal;
    QString m_swapUsed;
    QString m_swapFree;
    QString m_ioRead;
    QString m_ioWrite;
    QVariantList m_ioHistory;

    // Network data
    QString m_networkInterface;
    QString m_ipAddress;
    QString m_macAddress;
    QString m_netUpSpeed;
    QString m_netDownSpeed;
    QString m_packetRate;
    int m_activeConnections;
    QVariantList m_netUpHistory;
    QVariantList m_netDownHistory;

    // System data
    QString m_hostname;
    QString m_osVersion;
    QString m_kernelVersion;
    QString m_uptime;
    QString m_systemTime;

    // Settings
    int m_updateInterval;
    bool m_darkMode;
    bool m_soundAlert;
    int m_cpuWarnThreshold;
    int m_cpuCritThreshold;
    int m_ramWarnThreshold;

    // Logs
    QVariantList m_systemLogs;
};

#endif // SYSTEMCONTROLLER_H