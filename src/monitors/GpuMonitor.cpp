/**
 * ============================================
 * File: src/monitors/GpuMonitor.cpp
 * Description: GPU monitoring with vcgencmd support
 * ============================================
 */

#include "GpuMonitor.h"
#include "FileReader.h"
#include <QProcess>
#include <QDebug>

GpuMonitor::GpuMonitor(QObject *parent)
    : QObject(parent)
    , m_vcgencmdAvailable(false)
{
    // Check if vcgencmd is available 
    m_vcgencmdAvailable = isVcgencmdAvailable();

    if (!m_vcgencmdAvailable) {
        qWarning() << "vcgencmd not available - using fallback methods";
    } else {
        qInfo() << "vcgencmd detected - using real GPU metrics";
    }
}

int GpuMonitor::parseTemp()
{
    if (m_vcgencmdAvailable) {
        // Try to get GPU temp using vcgencmd
        QProcess process;
        process.start("vcgencmd", QStringList() << "measure_temp");

        if (process.waitForFinished(1000)) {
            QString output = process.readAllStandardOutput().trimmed();
            // Output format: "temp=45.0'C"

            int startPos = output.indexOf('=');
            int endPos = output.indexOf('\'');
            
            if (startPos > 0 && endPos > startPos) {
                QString tempStr = output.mid(startPos +1, endPos - startPos - 1);
                bool ok;
                double temp = tempStr.toDouble(&ok);

                if (ok) {
                    return static_cast<int>(temp);
                }
            }
        }
    }

    // Fallback: Use thermal_zone0 (same as CPU temp on Pi)
    QString TempStr = FileReader::readFirstLine("/sys/class/thermal/thermal_zone0/temp");

    if (TempStr.isEmpty()) {
        return 0;
    }

    bool ok;
    int tempMilliDegrees = TempStr.toInt(&ok);

    if (!ok) {
        return 0;
    }

    return tempMilliDegrees / 1000;
}

int GpuMonitor::parseMemUsage()
{
    if (!m_vcgencmdAvailable) {
        // Return -1 to signal "not available"
        // QML will hide GPU card when < 0
        return -1;
    }

    // Try to get GPU memory using vcgencmd
    QProcess process;
    process.start("vcgencmd", QStringList() << "get_mem" << "gpu");

    if (!process.waitForFinished(1000)) {
        return -1;
    }

    QString output = process.readAllStandardOutput().trimmed();
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

bool GpuMonitor::isVcgencmdAvailable()
{
    // Check if vcgencmd binary exists
    QProcess process;
    process.start("which", QStringList() << "vcgencmd"); 

    if (!process.waitForFinished(1000)) {
        return false;
    }

    QString output= process.readAllStandardOutput().trimmed();
    return !output.isEmpty();
}
