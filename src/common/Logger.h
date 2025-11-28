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
    Q_OBJECT

    // QML Properties
    Q_PROPERTY(QStringList logs READ logs NOTIFY logsChanged)
    Q_PROPERTY(int maxLogEntries READ maxLogEntries WRITE setMaxLogEntries NOTIFY maxLogEntriesChanged)

public:
    /**
     * @brief Log severity levels
     */

    enum Level {
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };
    Q_ENUM(Level)

    /**
     * @brief Get singleton instance
     */
    static Logger& instance();

    /**
     * @brief Log a message with specified level
     */
    void log(Level level, const QString& message, const QString& source = QString());

    /**
     * @brief Log a message with specified level
     */
    void debug(const QString& message, const QString& source = QString());
    void info(const QString& message, const QString& source = QString());
    void warning(const QString& message, const QString& source = QString());
    void error(const QString& message, const QString& source = QString());
    void critical(const QString& message, const QString& source = QString());

    /**
     * @brief QML invokable logging
     */
    Q_INVOKABLE void addLog(const QString& level, const QString& message);

    /**
     * @brief Get all log entries
     */
    QStringList logs() const;

    /**
     * @brief Clear all logs
     */
    Q_INVOKABLE void clearLogs();

    /**
     * @brief Export logs to file
     */
    Q_INVOKABLE bool exportLogs(const QString& filePath);

    /**
     * @brief Max log entries property
     */
    int maxLogEntries() const;
    void setMaxLogEntries(int max);

    /**
     * @brief Enable/disable file logging
     */
    void setFileLogginngEnabled(bool enabled);
    void setLogFilePath(const QString& path);

signals:
    void logChanged();
    void maxLogEntriesChanged();
    void newLogEntry(const QString& entry);

private:
    explicit Logger(QObject* parent = nullptr);
    ~Logger();

    // Prevent copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    QString levelToString(Level level) const;
    QString formatLogEntry(Level level, const QString& message, const QString& source) const;
    void writeToFile(const QString& entry);
    void trimLogs();

private:
    QStringList m_logs;
    int m_maxLogEntries;
    QMutex m_mutex;

    bool m_fileLoggingEnabled;
    QString m_logFilePath;
    QFile* m_logFile;
};

// ============================================================================
// Convenience Macros
// ============================================================================

#define LOG_DEBUG(msg)      Logger::instance().debug(msg, __FUNCTION__)
#define LOG_INFO(msg)       Logger::instance().info(msg, __FUNCTION__)
#define LOG_WARNING(msg)    Logger::instance().warning(msg, __FUNCTION__)
#define LOG_ERROR(msg)      Logger::instance().error(msg, __FUNCTION__)
#define LOG_CRITICAL(msg)   Logger::instance().critical(msg, __FUNCTION__)

#endif //LOGGER_H