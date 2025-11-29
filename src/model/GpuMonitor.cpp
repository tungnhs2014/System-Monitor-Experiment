/**
 * ============================================
 * File: src/model/GpuMonitor.cpp
 * Description: GPU monitoring implementation
 * ============================================
 */

#include "GpuMonitor.h"
#include "FileReader.h"
#include "Logger.h"
#include <QProcess>

GpuMonitor::GpuMonitor(QObject *parent)
    : QObject(parent)
    , m_vcgencmdAvailable(false)
{
    m_vcgencmdAvailable = checkVcgencmdAvailable();

    if (!m_vcgencmdAvailable) {
        LOG_INFO("GpuMonitor: vcgencmd available - using real GPU metrics");
    }
    else {
        LOG_INFO("GpuMonitor: vcgencmd not available - using fallback");
    }
}

int GpuMonitor::parseTemp()
{
    if (m_vcgencmdAvailable) {
        // Use vcgencmd for accurate GPU temperature
        QString output = executeVcgencmnd({"measure_temp"});

        // Output format: "temp=45.0'C"
        int startPos = output.indexOf('=');
        int endPos = output.indexOf('\'');

        if (startPos > 0 && endPos > startPos) {
            QString tempStr = output.mid(startPos + 1, endPos - startPos - 1);
            bool ok;
            double temp = tempStr.toDouble(&ok);

            if (ok) {
                return static_cast<int>(temp);
            }
        }
    }

    // Fallback: Use thermal zone0 (same as CPU on Pi)
    int tempMilliDegrees = FileReader::readInt("/sys/class/thermal/thermal_zone0/temp", 0);

    if (tempMilliDegrees > 0) {
        return tempMilliDegrees / 1000;
    }

    return 0;
}

int GpuMonitor::parseMemUsage()
{
    if (!m_vcgencmdAvailable) {
        return -1; // Signal "not available"
    }

    QString output = executeVcgencmnd({"get_mem", "gpu"});

    // Output format: "gpu=256M"
    int equalPos = output.indexOf('=');
    int mPos = output.indexOf('M');

    if (equalPos > 0 && mPos > equalPos) {
        QString memStr = output.mid(equalPos + 1, mPos - equalPos - 1);
        bool ok;
        int mem = memStr.toInt(&ok);

        if (ok) {
            return mem;
        }
    }

    return -1;
}

int GpuMonitor::parseClockFreq()
{
    if (!m_vcgencmdAvailable) {
        return -1;
    }

    QString output = executeVcgencmnd({"measure_clock", "core"});

    // Output format: "frequency(48)=500000000" (in Hz)
    int equalPos = output.indexOf('=');

    if (equalPos > 0) {
        QString freqStr = output.mid(equalPos + 1).trimmed();
        bool ok;
        long long freqHz = freqStr.toLongLong(&ok);

        if (ok) {
            // Convert Hz to MHZ
            return static_cast<int>(freqHz / 1000000);
        }
    }

    return -1;
}

bool GpuMonitor::checkVcgencmdAvailable()
{
    // Check if vcgencmd binary exists
    QProcess process;
    process.start("which", {"vcgencmd"}); 

    if (!process.waitForFinished(1000)) {
        return false;
    }

    QString output = process.readAllStandardOutput().trimmed();
    return !output.isEmpty();
}

QString GpuMonitor::executeVcgencmnd(const QStringList &args)
{
    QProcess process;
    process.start("vcgencmd", args);

    if (!process.waitForFinished(1000)) {
        return QString();
    }

    return QString::fromUtf8(process.readAllStandardOutput().trimmed());
}
