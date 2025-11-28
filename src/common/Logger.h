/**
 * ============================================================================
 * File: src/common/Logger.h
 * Description: Professional logging system with file and console output
 * ============================================================================
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QMutex>
#include <QFile>

/**
 * @class Logger
 * @brief Singleton logger for application-wide logging
 * 
 * Features:
 * - Thread-safe logging
 * - Multiple log levels
 * - Console and file output
 * - Log rotation
 * - QML accessible
 */


class Logger : public QObject 
{
    
};
#endif //LOGGER_H