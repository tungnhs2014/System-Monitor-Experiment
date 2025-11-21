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
    , m_hddUsage(0)
    , m_hddTemp(0)
    , m_netDown("0.0")
    , m_netUp("0.0")
{
    // Create monitor instances
    m_cpuMonitor = new CpuMonitor(this);
    m_gpuMonitor = new GpuMonitor(this);
    m_memMonitor = new MemoryMonitor(this);
    m_storageMonitor = new StorageMonitor(this);
    m_networkMonitor = new NetworkMonitor(this);

    // Create Update timer (2 seconds interval)
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(2000);
    connect(m_updateTimer, &QTimer::timeout, this, &SystemInfo::updateAllMetrics);

    // Inital update
    updateAllMetrics();

    // Start timer
    m_updateTimer->start();

    qDebug() << "System Info initialized - update interval: 2s";
}

void SystemInfo::updateAllMetrics()
{
    // Update CPU metrics
    setCpuUsages(m_cpuMonitor->parseUsage());
    setCpuTemp(m_cpuMonitor->parseTemp());
    setCpuClock(m_cpuMonitor->parseClock());

    // Update GPU metrics
    setGpuTemp(m_gpuMonitor->parseTemp());
    setGpuMemUsage(m_gpuMonitor->parseMemUsage());
    // GPU usage is set to same as CPU for now (Pi doesn't have separate GPU usage)
    setGpuUsages(m_cpuUsage);

    // Update RAM metrics
    setRamUsage(m_memMonitor->parseUsage());
    setRamUsed(m_memMonitor->parseUsed());
    setRamFree(m_memMonitor->parseFree());

    // Update Storage metrics
    setHddUsage(m_storageMonitor->parseUsage());
    setHddTemp(m_storageMonitor->parseTemp());

    // Update Network metrics
    setNetDown(m_networkMonitor->parseDownloadRate());
    setNetUp(m_networkMonitor->parseUploadRate());
}

// CPU Setters
void SystemInfo::setCpuUsages(int value) 
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

// GPU Setters
void SystemInfo::setGpuUsages(int value)
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

// RAM Setters
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

// Storage Setters
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

// Network Setters
void SystemInfo::setNetDown(const QString &value)
{
    if (m_netDown != value) {
        m_netDown = value;
        emit netDownChanged();
    }
}

void SystemInfo::setNetUp(const QString &value)
{
    if (m_netUp != value) {
        m_netUp = value;
        emit netUpChanged();
    }
}
