/**
 * ============================================================================
 * File: src/model/GpuMonitor.h
 * Description: GPU monitoring for Raspberry Pi 
 * ============================================================================
 */

#ifndef GPUMONITOR_H
#define GPUMONITOR_H

#include <QObject>

/**
 * @class GpuMonitor
 * @brief Monitors Raspberry Pi VideoCore GPU temperature and memory
 * 
 * Uses vcgencmd for real GPU metrics on Raspberry Pi
 * Falls back to thermal_zone on other platforms
 */
class GpuMonitor : public QObject
{
    Q_OBJECT

public:
    explicit GpuMonitor(QObject *parent = nullptr);
    ~GpuMonitor() override = default;

     /**
     * @brief Parse GPU temperature
     * @return Temperature in Celsius
     */
    int parseTemp();

    /**
     * @brief Parse GPU memory allocation
     * @return GPU memory in MB, or -1 if not available
     */
    int parseMemUsage();

    /**
     * @brief Check if GPU monitoring is available
     * @return true if vcgencmd is available
     */
    bool isAvailable() const { return m_vcgencmdAvailable; }

    /**
     * @brief Get GPU clock frequency
     * @return Frequency in MHz, or -1 if not available
     */
    int parseClockFreq();

private:
    /**
     * @brief Check if vcgencmd is available on system
     */
    bool checkVcgencmdAvailable();

    /**
     * @brief Execute vcgencmd with arguments
     * @param args Command arguments
     * @return Command output
     */
    QString executeVcgencmd(const QStringList& args);

private:
    bool m_vcgencmdAvailable;
};

#endif // GPUMONITOR_H