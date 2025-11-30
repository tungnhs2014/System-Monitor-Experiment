/**
 * ============================================================================
 * File: src/common/Logger.h
 * Description: High-performance logging system with compile-time optimization
 * 
 * LOG_LEVEL (defined in CMakeLists.txt):
 *   0 = OFF      - No logging (Release)
 *   1 = CRITICAL - Critical errors only
 *   2 = ERROR    - Errors and above
 *   3 = WARNING  - Warnings and above
 *   4 = INFO     - Info and above
 *   5 = DEBUG    - All messages (Debug)
 * 
 * Usage:
 *   LOG_DEBUG("Debug message");
 *   LOG_INFO("Info message");
 *   LOG_WARNING("Warning message");
 *   LOG_ERROR("Error message");
 *   LOG_CRITICAL("Critical message");
 * ============================================================================
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

// ============================================================================
// Log Level Constants
// ============================================================================
#define LOG_LEVEL_OFF      0
#define LOG_LEVEL_CRITICAL 1
#define LOG_LEVEL_ERROR    2
#define LOG_LEVEL_WARNING  3
#define LOG_LEVEL_INFO     4
#define LOG_LEVEL_DEBUG    5

// Default to OFF if not defined
#ifndef LOG_LEVEL
    #define LOG_LEVEL LOG_LEVEL_OFF
#endif

// ============================================================================
// Compile-Time Log Macros (Zero overhead when disabled)
// ============================================================================

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
    #define LOG_DEBUG(msg)    Logger::instance().debug(msg)
#else
    #define LOG_DEBUG(msg)    ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
    #define LOG_INFO(msg)     Logger::instance().info(msg)
#else
    #define LOG_INFO(msg)     ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARNING
    #define LOG_WARNING(msg)  Logger::instance().warning(msg)
    #define LOG_WARN(msg)     Logger::instance().warning(msg)
#else
    #define LOG_WARNING(msg)  ((void)0)
    #define LOG_WARN(msg)     ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
    #define LOG_ERROR(msg)    Logger::instance().error(msg)
#else
    #define LOG_ERROR(msg)    ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_CRITICAL
    #define LOG_CRITICAL(msg) Logger::instance().critical(msg)
    #define LOG_CRIT(msg)     Logger::instance().critical(msg)
#else
    #define LOG_CRITICAL(msg) ((void)0)
    #define LOG_CRIT(msg)     ((void)0)
#endif

// ============================================================================
// Logger Class (Only compiled when LOG_LEVEL > 0)
// ============================================================================

#if LOG_LEVEL > LOG_LEVEL_OFF

#include <QObject>
#include <QMutex>
#include <QFile>
#include <QVariantList>

/**
 * @class Logger
 * @brief Thread-safe singleton logger with file and console output
 */
class Logger : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList logs READ logs NOTIFY logsChanged)

public:
    /**
     * @brief Get singleton instance
     */
    static Logger& instance();

    // Delete copy/move
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    // ==================== Logging Methods ====================
    void debug(const QString& message);
    void info(const QString& message);
    void warning(const QString& message);
    void error(const QString& message);
    void critical(const QString& message);

    // ==================== Configuration ====================
    void setLogToFile(bool enabled);
    void setLogFilePath(const QString& path);
    void setMaxLogEntries(int max);
    void setLogLevel(int level);
    
    int logLevel() const { return m_logLevel; }
    bool isLogToFile() const { return m_logToFile; }

    // ==================== QML Access ====================
    QVariantList logs() const { return m_logs; }
    Q_INVOKABLE void clearLogs();
    Q_INVOKABLE QString getLogLevelName() const;

signals:
    void logsChanged();
    void newLogEntry(const QString& level, const QString& message);

private:
    Logger();
    ~Logger() override;

    void log(const QString& level, const QString& message);
    void writeToFile(const QString& formattedMessage);
    void rotateLogFile();
    QString formatMessage(const QString& level, const QString& message);

private:
    QMutex m_mutex;
    QVariantList m_logs;
    QFile m_logFile;
    
    int m_logLevel;
    int m_maxLogEntries;
    bool m_logToFile;
    QString m_logFilePath;
    
    static constexpr int DEFAULT_MAX_ENTRIES = 100;
    static constexpr qint64 MAX_FILE_SIZE = 1024 * 1024;  // 1MB
};

#else  // LOG_LEVEL == LOG_LEVEL_OFF

/**
 * @brief Stub Logger class for Release builds
 * Provides minimal interface with zero overhead
 */
class Logger
{
public:
    static Logger& instance() {
        static Logger inst;
        return inst;
    }
    
    // Empty implementations - will be optimized out
    inline void debug(const QString&) {}
    inline void info(const QString&) {}
    inline void warning(const QString&) {}
    inline void error(const QString&) {}
    inline void critical(const QString&) {}
    
    inline void setLogToFile(bool) {}
    inline void setLogFilePath(const QString&) {}
    inline void setMaxLogEntries(int) {}
    inline void setLogLevel(int) {}
    
private:
    Logger() = default;
};

#endif  // LOG_LEVEL > LOG_LEVEL_OFF

#endif  // LOGGER_H