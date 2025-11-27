/**
 * ============================================
 * File: src/SystemInfo.cpp
 * Description: Main system info aggregator implementation
 * ============================================
 */

 #include "SystemInfo.h"
 #include <QDebug>

SystemInfo::SystemInfo(QObject *parent) 
    : QObject(parent)
    , m_cpuUsage(0)
    , m_cpuTemp(0)
    , m_cpuClock("0.0G")
    , m_gpuUsage(0)
    , m_gpuTemp(0)
    , m_gpuMemUsage(-1)
    , m_ramUsage(0)
    , m_ramUsed("0M")
    , m_ramFree("OM")
    , m_ramCache("0M")
    , m_hddUsage(0)
    , m_hddTemp(0)
    , m_hddTotal("00GB")
    , m_hddUsed("0GB")
    , m_hddFree("0GB")
    , m_swapUsage(0)
    , m_swapTotal("0MB")
    , m_swapUsed("0MB")
    , m_swapFree("0MB")
    , m_ioRead("0 MB/s")
    , m_ioWrite("0 MB/s")
    , m_networkInterface("N/A")
    , m_ipAddress("N/A")
    , m_macAddress("N/A")
    , m_netUpSpeed("0.0 Bps")
    , m_netDownSpeed("0.0 Bps")
    , m_packetRate("0 pps")
    , m_activeConnections(0)
    , m_hostname("unknown")
    , m_osVersion("Unknown OS")
    , m_kernelVersion("Unknown")
    , m_uptime("0s")
    , m_systemTime("N/A")
    , m_updateInterval(3)  // Increased to 3s for better performance on slow hardware
    , m_darkMode(true)
    , m_soundAlert(false)
    , m_cpuWarnThreshold(70)
    , m_cpuCritThreshold(90)
    , m_ramWarnThreshold(80)

{
    // ==================== CREATE MONITOR INSTANCES ====================
    m_cpuMonitor = new CpuMonitor(this);
    m_gpuMonitor = new GpuMonitor(this);
    m_memMonitor = new MemoryMonitor(this);
    m_storageMonitor = new StorageMonitor(this);
    m_networkMonitor = new NetworkMonitor(this);
    m_settingsManager = new SettingsManager(this);

    // ==================== INITIALIZE SYSTEM INFO FROM SETTINGS ====================
    m_hostname = m_settingsManager->getHostname();
    m_osVersion = m_settingsManager->getOsVersion();
    m_kernelVersion = m_settingsManager->getKernelVersion();
    m_updateInterval = m_settingsManager->getUpdateInterval();
    m_darkMode = m_settingsManager->getDarkMode();
    m_soundAlert = m_settingsManager->getSoundAlert();
    m_cpuWarnThreshold = m_settingsManager->getCpuWarnThreshold();
    m_cpuCritThreshold = m_settingsManager->getCpuCritThreshold();
    m_ramWarnThreshold = m_settingsManager->getRamWarnThreshold();

    // ==================== CONNECT SETTINGS SIGNALS ====================
    connect(m_settingsManager, &SettingsManager::logsChanged, this, [this]() {
        m_systemLogs = m_settingsManager->getSystemLogs();
        emit systemLogsChanged();
    });

    // Load initial logs
    m_systemLogs = m_settingsManager->getSystemLogs();

    // ==================== CREATE UPDATE TIMER ====================
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(m_updateInterval * 1000);
    connect(m_updateTimer, &QTimer::timeout, this, &SystemInfo::updateAllMetrics);

    // ==================== INITIAL UPDATE ====================
    updateAllMetrics();

    // ==================== START TIMER ====================
    m_updateTimer->start();

    qDebug() << "===========================================";
    qDebug() << "SystemInfo initialized - COMPLETE VERSION";
    qDebug() << "Update interval:" << m_updateInterval << "seconds";
    qDebug() << "Hostname:" << m_hostname;
    qDebug() << "OS:" << m_osVersion;
    qDebug() << "Kernel:" << m_kernelVersion;
    qDebug() << "===========================================";
}

void SystemInfo::updateAllMetrics()
{
    // ==================== UPDATE CPU METRICS ====================
    setCpuUsage(m_cpuMonitor->parseUsage());
    setCpuTemp(m_cpuMonitor->parseTemp());
    setCpuClock(m_cpuMonitor->parseClock());
    
    // Per-core usage
    m_coreUsages = m_cpuMonitor->parsePerCoreUsage();
    emit coreUsagesChanged();

    // Temperature history
    m_cpuMonitor->updateTempHistory(m_cpuTemp);
    m_tempHistory = m_cpuMonitor->getTempHistory();
    emit tempHistoryChanged();
        
    // ==================== UPDATE GPU METRICS ====================
    setGpuTemp(m_gpuMonitor->parseTemp());
    setGpuMemUsage(m_gpuMonitor->parseMemUsage());
    setGpuUsage(m_cpuUsage); // GPU usage same as CPU For Pi

    // ==================== UPDATE RAM METRICS ====================
    setRamUsage(m_memMonitor->parseUsage());
    setRamUsed(m_memMonitor->parseUsed());
    setRamFree(m_memMonitor->parseFree());

    // Cache
    m_ramCache = m_memMonitor->parseCache();
    emit ramCacheChanged();

    // ==================== UPDATE STORAGE METRICS ====================
    setHddUsage(m_storageMonitor->parseUsage());
    setHddTemp(m_storageMonitor->parseTemp());

    // Root partition detai
    m_storageMonitor->updateRootPartition();
    m_hddTotal = m_storageMonitor->getRootTotal();
    m_hddUsed = m_storageMonitor->getRootUsed();
    m_hddFree = m_storageMonitor->getRootFree();
    emit hddTotalChanged();
    emit hddUsedChanged();
    emit hddFreeChanged();

    // Swap
    m_storageMonitor->updateSwap();
    m_swapUsage = m_storageMonitor->getSwapUsage();
    m_swapTotal = m_storageMonitor->getSwapTotal();
    m_swapUsed = m_storageMonitor->getSwapUsed();
    m_swapFree = m_storageMonitor->getSwapFree();
    emit swapUsageChanged();
    emit swapTotalChanged();
    emit swapUsedChanged();
    emit swapFreeChanged();
    
    // I/O statistics ()
    m_storageMonitor->updateIoStats();
    m_ioRead = m_storageMonitor->getIoRead();
    m_ioWrite = m_storageMonitor->getIoWrite();
    m_ioHistory = m_storageMonitor->getIoHistory();
    emit ioReadChanged();
    emit ioWriteChanged();
    emit ioHistoryChanged();

    // ==================== UPDATE NETWORK METRICS ====================
    m_networkMonitor->update();

    m_networkInterface = m_networkMonitor->getInterface();
    m_ipAddress = m_networkMonitor->getIpAddress();
    m_macAddress = m_networkMonitor->getMacAddress();
    m_netUpSpeed = m_networkMonitor->getUpSpeed();
    m_netDownSpeed = m_networkMonitor->getDownSpeed();
    m_netUpHistory = m_networkMonitor->getUpHistory();
    m_netDownHistory = m_networkMonitor->getDownHistory();
    m_packetRate = m_networkMonitor->getPacketRate();
    m_activeConnections = m_networkMonitor->getActiveConnections();
    
    emit networkInterfaceChanged();
    emit ipAddressChanged();
    emit macAddressChanged();
    emit netUpSpeedChanged();
    emit netDownSpeedChanged();
    emit netUpHistoryChanged();
    emit netDownHistoryChanged();
    emit packetRateChanged();
    emit activeConnectionsChanged();

    // ==================== UPDATE SYSTEM INFO ====================
    m_uptime = m_settingsManager->getupTime();
    m_systemTime = m_settingsManager->getSystemTime();
    emit uptimeChanged();
    emit systemTimeChanged();

    // ==================== CHECK WARNING THRESHOLDS ====================
    if (m_cpuUsage >= m_cpuCritThreshold) {
        m_settingsManager->addLog("CRITICAL", QString("CPU usage critical: %1%").arg(m_cpuUsage));
    }
    else if (m_cpuUsage >= m_cpuWarnThreshold) {
        m_settingsManager->addLog("WARNING", QString("CPU usage high: %1%").arg(m_cpuUsage));
    }
    
    if (m_ramUsage >= m_ramWarnThreshold) {
        m_settingsManager->addLog("WARNING", QString("RAM usage high: %1%").arg(m_ramUsage));
    }
}

// ==================== SETTINGS SETTERS ====================

void SystemInfo::setUpdateInterval(int interval)
{
    if (m_updateInterval != interval) {
        m_updateInterval = interval;
        m_settingsManager->setUpdateInterval(interval);
        m_updateTimer->setInterval(interval * 1000);
        emit updateIntervalChanged();
    }
}

void SystemInfo::setDarkMode(bool enabled)
{
    if (m_darkMode != enabled) {
        m_darkMode = enabled;
        m_settingsManager->setDarkMode(enabled);
        emit darkModeChanged();
    }
}

void SystemInfo::setSoundAlert(bool enabled)
{
    if (m_soundAlert != enabled) {
        m_soundAlert = enabled;
        m_settingsManager->setSoundAlert(enabled);
        emit soundAlertChanged();
    }
}

void SystemInfo::setCpuWarnThreshold(int threshold)
{
    if (m_cpuWarnThreshold != threshold) {
        m_cpuWarnThreshold = threshold;
        m_settingsManager->setCpuWarnThreshold(threshold);
        emit cpuWarnThresholdChanged();
    }
}

void SystemInfo::setCpuCritThreshold(int threshold)
{
    if (m_cpuCritThreshold != threshold) {
        m_cpuCritThreshold = threshold;
        m_settingsManager->setCpuCritThreshold(threshold);
        emit cpuCritThresholdChanged();
    }
}

void SystemInfo::setRamWarnThreshold(int threshold)
{
    if (m_ramWarnThreshold != threshold) {
        m_ramWarnThreshold = threshold;
        m_settingsManager->setRamWarnThreshold(threshold);
        emit ramWarnThresholdChanged();
    }
}

// ==================== INVOKABLE METHODS ====================

void SystemInfo::saveSettings() {
    m_settingsManager->save();
}

void SystemInfo::reboot() {
    m_settingsManager->reboot();
}

void SystemInfo::shutdown() {
    m_settingsManager->shutdown();
}

// ==================== HELPER SETTERS ====================

void SystemInfo::setCpuUsage(int value) 
{
    if (m_cpuUsage != value) {
        m_cpuUsage = value;
        emit cpuUsageChanged();
    }
}

void SystemInfo::setCpuTemp(int value)
{
    if (m_cpuTemp != value) {
        m_cpuTemp = value;
        emit cpuTempChanged();
    }
}

void SystemInfo::setCpuClock(const QString &value)
{
    if (m_cpuClock != value) {
        m_cpuClock = value;
        emit cpuClockChanged();
    }
}

void SystemInfo::setGpuUsage(int value)
{
    if (m_gpuUsage != value) {
        m_gpuUsage = value;
        emit gpuUsageChanged();
    }
}

void SystemInfo::setGpuTemp(int value)
{
    if (m_gpuTemp != value) {
        m_gpuTemp = value;
        emit gpuTempChanged();
    }
}

void SystemInfo::setGpuMemUsage(int value)
{
    if (m_gpuMemUsage != value) {
        m_gpuMemUsage = value;
        emit gpuMemUsageChanged();
    }
}

void SystemInfo::setRamUsage(int value)
{
    if (m_ramUsage != value) {
        m_ramUsage = value;
        emit ramUsageChanged();
    }
}

void SystemInfo::setRamUsed(const QString &value)
{
    if (m_ramUsed != value) {
        m_ramUsed = value;
        emit ramUsedChanged();
    }
}

void SystemInfo::setRamFree(const QString &value)
{
    if (m_ramFree != value) {
        m_ramFree = value;
        emit ramFreeChanged();
    }
}

void SystemInfo::setHddUsage(int value)
{
    if (m_hddUsage != value) {
        m_hddUsage = value;
        emit hddUsageChanged();
    }
}

void SystemInfo::setHddTemp(int value)
{
    if (m_hddTemp != value) {
        m_hddTemp = value;
        emit hddTempChanged();
    }
}


