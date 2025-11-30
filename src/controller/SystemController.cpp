/**
 * ============================================================================
 * File: src/controller/SystemController.cpp
 * Description: Main controller implementation
 * ============================================================================
 */

#include "SystemController.h"

#include "CpuMonitor.h"
#include "GpuMonitor.h"
#include "MemoryMonitor.h"
#include "StorageMonitor.h"
#include "NetworkMonitor.h"
#include "SettingsManager.h"

#include "Constants.h"
#include "Logger.h"
#include <QDebug>

SystemController::SystemController(QObject* parent) 
    : QObject(parent)
    , m_cpuMonitor(nullptr)
    , m_gpuMonitor(nullptr)
    , m_memMonitor(nullptr)
    , m_storageMonitor(nullptr)
    , m_networkMonitor(nullptr)
    , m_settingsManager(nullptr)
    , m_updateTimer(nullptr)
    // Initialize CPU
    , m_cpuUsage(0)
    , m_cpuTemp(0)
    , m_cpuClock("N/A")
    , m_loadAverage("N/A")
    // Initialize GPU
    , m_gpuUsage(0)
    , m_gpuTemp(0)
    , m_gpuMemUsage(-1)
    // Initialize RAM
    , m_ramUsage(0)
    , m_ramUsed("0M")
    , m_ramFree("0M")
    , m_ramCache("0M")
    , m_ramTotal(0)
    // Initialize Storage
    , m_hddUsage(0)
    , m_hddTemp(0)
    , m_hddTotal("0GB")
    , m_hddUsed("0GB")
    , m_hddFree("0GB")
    , m_swapUsage(0)
    , m_swapTotal("0M")
    , m_swapUsed("0M")
    , m_swapFree("0M")
    , m_ioRead("0 MB/s")
    , m_ioWrite("0 MB/s")
    // Initialize Network
    , m_networkInterface("N/A")
    , m_ipAddress("N/A")
    , m_macAddress("N/A")
    , m_netUpSpeed("0 B/s")
    , m_netDownSpeed("0 B/s")
    , m_packetRate("0 pps")
    , m_activeConnections(0)
    // Initialize System
    , m_hostname("unknown")
    , m_osVersion("Unknown")
    , m_kernelVersion("Unknown")
    , m_uptime("0s")
    , m_systemTime("N/A")
    // Initialize Settings
    , m_updateInterval(2)
    , m_darkMode(true)
    , m_soundAlert(false)
    , m_cpuWarnThreshold(App::Threshold::CPU_WARNING)
    , m_cpuCritThreshold(App::Threshold::CPU_CRITICAL)
    , m_ramWarnThreshold(App::Threshold::RAM_WARNING)
{
    initializeMonitors();
    connectSignals();

    // Load system info from settings
    m_hostname = m_settingsManager->hostname();
    m_osVersion = m_settingsManager->osVersion();
    m_kernelVersion = m_settingsManager->kernelVersion();

    // Load user settings
    m_updateInterval = m_settingsManager->updateInterval();
    m_darkMode = m_settingsManager->darkMode();
    m_soundAlert = m_settingsManager->soundAlert();
    m_cpuWarnThreshold = m_settingsManager->cpuWarnThreshold();
    m_cpuCritThreshold = m_settingsManager->cpuCritThreshold();
    m_ramWarnThreshold = m_settingsManager->ramWarnThreshold();

    // Create update timer
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(m_updateInterval * 1000);
    connect(m_updateTimer, &QTimer::timeout, this, &SystemController::updateAllMetrics);

    // Initial update
    updateAllMetrics();

    // Start timer
    m_updateTimer->start();

    LOG_INFO("SystemController initialized");
    LOG_INFO(QString("Update interval: %1 seconds").arg(m_updateInterval));
    LOG_INFO(QString("Hostname: %1").arg(m_hostname));
    LOG_INFO(QString("OS: %1").arg(m_osVersion));
    LOG_INFO(QString("Kernel: %1").arg(m_kernelVersion));
}

SystemController::~SystemController()
{
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
}

void SystemController::initializeMonitors()
{
    m_cpuMonitor = new CpuMonitor(this);
    m_gpuMonitor = new GpuMonitor(this);
    m_memMonitor = new MemoryMonitor(this);
    m_storageMonitor = new StorageMonitor(this);
    m_networkMonitor = new NetworkMonitor(this);
    m_settingsManager = new SettingsManager(this);
}

void SystemController::connectSignals()
{
    // Connect settings logs to our logs
    connect(m_settingsManager, &SettingsManager::logsChanged, this, [this]() {
        m_systemLogs = m_settingsManager->systemLogs();
        emit systemLogsChanged();
    });

    // Load initial logs
    m_systemLogs = m_settingsManager->systemLogs();
}

void SystemController::updateAllMetrics() 
{
    // ==================== CPU ====================
    int newCpuUsage = m_cpuMonitor->parseUsage();
    if (m_cpuUsage != newCpuUsage) {
        m_cpuUsage = newCpuUsage;
        emit cpuUsageChanged();
    }
    
    int newCpuTemp = m_cpuMonitor->parseTemp();
    if (m_cpuTemp != newCpuTemp) {
        m_cpuTemp = newCpuTemp;
        emit cpuTempChanged();
    }

    QString newCpuClock = m_cpuMonitor->parseClock();
    if (m_cpuClock != newCpuClock) {
        m_cpuClock = newCpuClock;
        emit cpuClockChanged();
    }

    // Per-core usage
    m_coreUsages = m_cpuMonitor->parsePerCoreUsage();
    emit coreUsagesChanged();

    // Temperature history 
    m_cpuMonitor->updateTempHistory(m_cpuTemp);
    m_tempHistory = m_cpuMonitor->getTempHistory();
    emit tempHistoryChanged();

    // Load average
    QString newLoadAverage = m_cpuMonitor->parseLoadAverage();
    if (m_loadAverage != newLoadAverage) {
        m_loadAverage = newLoadAverage;
        emit loadAverageChanged();
    }

    // ==================== GPU ====================
    int newGpuTemp = m_gpuMonitor->parseTemp();
    if (m_gpuTemp != newGpuTemp) {
        m_gpuTemp = newGpuTemp;
        emit gpuTempChanged();
    }

    int newGpuMem = m_gpuMonitor->parseMemUsage();
    if (m_gpuMemUsage != newGpuMem) {
        m_gpuMemUsage = newGpuMem;
        emit gpuMemUsageChanged();
    }

    // GPU usage = CPU usage for Pi (shared)
    if (m_gpuUsage != m_cpuUsage) {
        m_gpuUsage = m_cpuUsage;
        emit gpuUsageChanged();
    }

    // ==================== RAM ====================
    int newRamUsage = m_memMonitor->parseUsage();
    if (m_ramUsage != newRamUsage) {
        m_ramUsage = newRamUsage;
        emit ramUsageChanged();
    }

    QString newRamUsed = m_memMonitor->parseUsed();
    if (m_ramUsed != newRamUsed) {
        m_ramUsed = newRamUsed;
        emit ramUsedChanged();
    }

    QString newRamFree = m_memMonitor->parseFree();
    if (m_ramFree != newRamFree) {
        m_ramFree = newRamFree;
        emit ramFreeChanged();
    }

    QString newRamCache = m_memMonitor->parseCache();
    if (m_ramCache != newRamCache) {
        m_ramCache = newRamCache;
        emit ramCacheChanged();
    }

    int newRamTotal = m_memMonitor->parseTotal();
    if (m_ramTotal != newRamTotal) {
        m_ramTotal = newRamTotal;
        emit ramTotalChanged();
    }

    // ==================== Storage ====================
    int newHddUsage = m_storageMonitor->parseUsage();
    if (m_hddUsage != newHddUsage) {
        m_hddUsage = newHddUsage;
        emit hddUsageChanged();
    }

    int newHddTemp = m_storageMonitor->parseTemp();
    if (m_hddTemp != newHddTemp) {
        m_hddTemp = newHddTemp;
        emit hddTempChanged();
    }

    // Root partition
    m_storageMonitor->updateRootPartition();
    
    QString newHddTotal = m_storageMonitor->getRootTotal();
    if (m_hddTotal != newHddTotal) {
        m_hddTotal = newHddTotal;
        emit hddTotalChanged();
    }

    QString newHddUsed = m_storageMonitor->getRootUsed();
    if (m_hddUsed != newHddUsed) {
        m_hddUsed = newHddUsed;
        emit hddUsedChanged();
    }

    QString newHddFree = m_storageMonitor->getRootFree();
    if (m_hddFree != newHddFree) {
        m_hddFree = newHddFree;
        emit hddFreeChanged();
    }

    // Swap
    m_storageMonitor->updateSwap();

    int newSwapUsage = m_storageMonitor->getSwapUsage();
    if (m_swapUsage != newSwapUsage) {
        m_swapUsage = newSwapUsage;
        emit swapUsageChanged();
    }

    m_swapTotal = m_storageMonitor->getSwapTotal();
    emit swapTotalChanged();
    
    m_swapUsed = m_storageMonitor->getSwapUsed();
    emit swapUsedChanged();
    
    m_swapFree = m_storageMonitor->getSwapFree();
    emit swapFreeChanged();

    // I/O
    m_storageMonitor->updateIoStats();
    
    m_ioRead = m_storageMonitor->getIoRead();
    emit ioReadChanged();
    
    m_ioWrite = m_storageMonitor->getIoWrite();
    emit ioWriteChanged();
    
    m_ioHistory = m_storageMonitor->getIoHistory();
    emit ioHistoryChanged();

    // ==================== Network ====================
    m_networkMonitor->update();

    m_networkInterface = m_networkMonitor->getInterface();
    emit networkInterfaceChanged();

    m_ipAddress = m_networkMonitor->getIpAddress();
    emit ipAddressChanged();

    m_macAddress = m_networkMonitor->getMacAddress();
    emit macAddressChanged();

    m_netUpSpeed = m_networkMonitor->getUpSpeed();
    emit netUpSpeedChanged();

    m_netDownSpeed = m_networkMonitor->getDownSpeed();
    emit netDownSpeedChanged();

    m_packetRate = m_networkMonitor->getPacketRate();
    emit packetRateChanged();

    m_activeConnections = m_networkMonitor->getActiveConnections();
    emit activeConnectionsChanged();

    m_netUpHistory = m_networkMonitor->getUpHistory();
    emit netUpHistoryChanged();

    m_netDownHistory = m_networkMonitor->getDownHistory();
    emit netDownHistoryChanged();

    // ==================== System ====================
    m_uptime = m_settingsManager->uptime();
    emit uptimeChanged();

    m_systemTime = m_settingsManager->systemTime();
    emit systemTimeChanged();

    // ==================== Check Thresholds ====================
    checkThresholds();
}

void SystemController::checkThresholds()
{
    static int lastCpuWarningLevel = 0;
    static int lastRamWarningLevel = 0;

    // CPU thresholds
    int cpuWarningLevel = 0;
    if (m_cpuUsage >= m_cpuCritThreshold) {
        cpuWarningLevel = 2;
    }
    else if (m_cpuUsage >= m_cpuWarnThreshold) {
        cpuWarningLevel = 1;
    }

    if (cpuWarningLevel != lastCpuWarningLevel) {
        if (cpuWarningLevel == 2) {
            m_settingsManager->addLog("CRIT", QString("CPU usage critical: %1%").arg(m_cpuUsage));
        }
        else if (cpuWarningLevel == 1) {
            m_settingsManager->addLog("WARN", QString("CPU usage high: %1%").arg(m_cpuUsage));
        }
        lastCpuWarningLevel = cpuWarningLevel;
    }

    // RAM thresholds
    int ramWarningLevel = (m_ramUsage >= m_ramWarnThreshold) ? 1 : 0;

    if (ramWarningLevel != lastRamWarningLevel) {
        if (ramWarningLevel == 1) {
            m_settingsManager->addLog("WARN", QString("RAM usage high: %1%").arg(m_ramUsage));
        }
        lastRamWarningLevel = ramWarningLevel;
    }
}

// ==================== Settings Setters ====================

void SystemController::setUpdateInterval(int interval)
{
    if (m_updateInterval != interval && interval >= 1 && interval <= 10) {
        m_updateInterval = interval;
        m_settingsManager->setUpdateInterval(interval);
        m_updateTimer->setInterval(interval * 1000);
        emit updateIntervalChanged();
    }
}

void SystemController::setDarkMode(bool enabled)
{
    if (m_darkMode != enabled) {
        m_darkMode = enabled;
        m_settingsManager->setDarkMode(enabled);
        emit darkModeChanged();
    }
}

void SystemController::setSoundAlert(bool enabled)
{
    if (m_soundAlert != enabled) {
        m_soundAlert = enabled;
        m_settingsManager->setSoundAlert(enabled);
        emit soundAlertChanged();
    }
}

void SystemController::setCpuWarnThreshold(int threshold)
{
    if (m_cpuWarnThreshold != threshold) {
        m_cpuWarnThreshold = threshold;
        m_settingsManager->setCpuWarnThreshold(threshold);
        emit cpuWarnThresholdChanged();
    }
}

void SystemController::setCpuCritThreshold(int threshold)
{
    if (m_cpuCritThreshold != threshold) {
        m_cpuCritThreshold = threshold;
        m_settingsManager->setCpuCritThreshold(threshold);
        emit cpuCritThresholdChanged();
    }
}

void SystemController::setRamWarnThreshold(int threshold)
{
    if (m_ramWarnThreshold != threshold) {
        m_ramWarnThreshold = threshold;
        m_settingsManager->setRamWarnThreshold(threshold);
        emit ramWarnThresholdChanged();
    }
}

// ==================== Invokable Methods ====================

void SystemController::saveSettings()
{
    m_settingsManager->save();
}

void SystemController::reboot()
{
    m_settingsManager->reboot();
}

void SystemController::shutdown()
{
    m_settingsManager->shutdown();
}

void SystemController::addLog(const QString& level, const QString& message)
{
    m_settingsManager->addLog(level, message);
}
