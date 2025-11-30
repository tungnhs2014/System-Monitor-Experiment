/**
 * ============================================================================
 * File: src/common/Logger.cpp
 * Description: Logger implementation
 * ============================================================================
 */

#include "Logger.h"

// Only compile implementation when logging is enabled
#if LOG_LEVEL > LOG_LEVEL_OFF

#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>

Logger::Logger()
    : QObject(nullptr)
    , m_logLevel(LOG_LEVEL)
    , m_maxLogEntries(DEFAULT_MAX_ENTRIES)
    , m_logToFile(false)
    , m_logFilePath("")
{
    // Set default log file path
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataPath);
    m_logFilePath = appDataPath + "/system-monitor.log";
}

Logger::~Logger()
{
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
}

Logger& Logger::instance()
{
    static Logger inst;
    return inst;
}

void Logger::debug(const QString& message)
{
    if (m_logLevel >= LOG_LEVEL_DEBUG) {
        log("DEBUG", message);
    }
}

void Logger::info(const QString& message)
{
    if (m_logLevel >= LOG_LEVEL_INFO) {
        log("INFO", message);
    }
}

void Logger::warning(const QString& message)
{
    if (m_logLevel >= LOG_LEVEL_WARNING) {
        log("WARN", message);
    }
}

void Logger::error(const QString& message)
{
    if (m_logLevel >= LOG_LEVEL_ERROR) {
        log("ERROR", message);
    }
}

void Logger::critical(const QString& message)
{
    if (m_logLevel >= LOG_LEVEL_CRITICAL) {
        log("CRIT", message);
    }
}

void Logger::log(const QString& level, const QString& message)
{
    QMutexLocker locker(&m_mutex);
    
    QString formatted = formatMessage(level, message);
    
    // Console output
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
    if (level == "DEBUG") {
        qDebug().noquote() << formatted;
    } else
#endif
#if LOG_LEVEL >= LOG_LEVEL_INFO
    if (level == "INFO") {
        qInfo().noquote() << formatted;
    } else
#endif
#if LOG_LEVEL >= LOG_LEVEL_WARNING
    if (level == "WARN") {
        qWarning().noquote() << formatted;
    } else
#endif
    {
        qCritical().noquote() << formatted;
    }
    
    // File output
    if (m_logToFile) {
        writeToFile(formatted);
    }
    
    // Store in memory for QML access
    QVariantMap entry;
    entry["time"] = QTime::currentTime().toString("hh:mm:ss");
    entry["level"] = level;
    entry["message"] = message;
    
    m_logs.prepend(entry);
    
    while (m_logs.size() > m_maxLogEntries) {
        m_logs.removeLast();
    }
    
    emit logsChanged();
    emit newLogEntry(level, message);
}

QString Logger::formatMessage(const QString& level, const QString& message)
{
    return QString("[%1] [%2] %3")
        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))
        .arg(level, -5)
        .arg(message);
}

void Logger::writeToFile(const QString& formattedMessage)
{
    // Check file size and rotate if needed
    if (m_logFile.isOpen() && m_logFile.size() > MAX_FILE_SIZE) {
        rotateLogFile();
    }
    
    // Open file if not open
    if (!m_logFile.isOpen()) {
        m_logFile.setFileName(m_logFilePath);
        if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            qWarning() << "Failed to open log file:" << m_logFilePath;
            m_logToFile = false;
            return;
        }
    }
    
    QTextStream stream(&m_logFile);
    stream << formattedMessage << "\n";
    stream.flush();
}

void Logger::rotateLogFile()
{
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
    
    // Rename old file
    QString backupPath = m_logFilePath + ".old";
    QFile::remove(backupPath);
    QFile::rename(m_logFilePath, backupPath);
    
    // Open new file
    m_logFile.setFileName(m_logFilePath);
    m_logFile.open(QIODevice::WriteOnly | QIODevice::Text);
}

void Logger::setLogToFile(bool enabled)
{
    QMutexLocker locker(&m_mutex);
    m_logToFile = enabled;
    
    if (!enabled && m_logFile.isOpen()) {
        m_logFile.close();
    }
}

void Logger::setLogFilePath(const QString& path)
{
    QMutexLocker locker(&m_mutex);
    
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
    
    m_logFilePath = path;
}

void Logger::setMaxLogEntries(int max)
{
    QMutexLocker locker(&m_mutex);
    m_maxLogEntries = qMax(10, max);
    
    while (m_logs.size() > m_maxLogEntries) {
        m_logs.removeLast();
    }
}

void Logger::setLogLevel(int level)
{
    QMutexLocker locker(&m_mutex);
    m_logLevel = qBound(LOG_LEVEL_OFF, level, LOG_LEVEL_DEBUG);
}

void Logger::clearLogs()
{
    QMutexLocker locker(&m_mutex);
    m_logs.clear();
    emit logsChanged();
}

QString Logger::getLogLevelName() const
{
    switch (m_logLevel) {
        case LOG_LEVEL_OFF:      return "OFF";
        case LOG_LEVEL_CRITICAL: return "CRITICAL";
        case LOG_LEVEL_ERROR:    return "ERROR";
        case LOG_LEVEL_WARNING:  return "WARNING";
        case LOG_LEVEL_INFO:     return "INFO";
        case LOG_LEVEL_DEBUG:    return "DEBUG";
        default:                 return "UNKNOWN";
    }
}

#endif  // LOG_LEVEL > LOG_LEVEL_OFF