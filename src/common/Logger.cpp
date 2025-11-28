/**
 * ============================================================================
 * File: src/common/Logger.cpp
 * Description: Logger implementation
 * ============================================================================
 */

#include "Logger.h"
#include "Constants.h"
#include <QDebug>
#include <QMutexLocker>
#include <QDir>

Logger::Logger(QObject *parent)
    : QObject(parent)
    , m_maxLogEntries(100)
    , m_fileLoggingEnabled(false)
    , m_logFile(nullptr)
{
}

Logger::~Logger()
{
    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
    }
}

Logger &Logger::instance()
{
    static Logger instance;
    return instance;
}

void Logger::log(Level level, const QString &message, const QString &source)
{
    QMutexLocker locker(&m_mutex);

    QString entry = formatLogEntry(level, message, source);

    // Add to internal log list
    m_logs.prepend(entry);
    trimLogs();

    // Output to console
    switch (level)
    {
        case Debug:
            qDebug().noquote() << entry;
            break;
        case Info:
            qInfo().noquote() << entry;
            break;
        case Warning:
            qWarning().noquote() << entry;
            break;
        case Error:
        case Critical:
            qCritical().noquote() << entry;
        default:
            break;
    }

    // Write to file if enabled
    if (m_fileLoggingEnabled) {
        writeToFile(entry);
    }

    emit logsChanged();
    emit newLogEntry(entry);
}

void Logger::debug(const QString &message, const QString &source)
{
    log(Debug, message, source);
}

void Logger::info(const QString &message, const QString &source)
{
    log(Info, message, source);
}

void Logger::warning(const QString &message, const QString &source)
{
    log(Warning, message, source);
}

void Logger::error(const QString &message, const QString &source)
{
    log(Error, message, source);
}

void Logger::critical(const QString &message, const QString &source)
{
    log(Critical, message, source);
}

void Logger::addLog(const QString &level, const QString &message)
{
    Level lvl = Info;

    if (level == "DEBUG") lvl = Debug;
    else if (level == "INFO") lvl = Info;
    else if (level == "WARN" || level == "WARNING") lvl = Warning;
    else if (level == "ERROR") lvl = Error;
    else if (level == "CRIT" || level == "CRITICAL") lvl = Critical;

    log(lvl, message, "QML");
}

QStringList Logger::logs() const
{
    return m_logs;
}

void Logger::clearLogs()
{
    QMutexLocker locker(&m_mutex);
    m_logs.clear();
    emit logsChanged();
}

bool Logger::exportLogs(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
    for (const QString &entry : m_logs) {
        stream << entry << "\n";
    }

    file.close();
    return true;
}

int Logger::maxLogEntries() const
{
    return m_maxLogEntries;
}

void Logger::setMaxLogEntries(int max)
{
    if (m_maxLogEntries != max) {
        m_maxLogEntries = max;
        trimLogs();
        emit maxLogEntriesChanged();
    }
}

void Logger::setFileLogginngEnabled(bool enabled)
{
    m_fileLoggingEnabled = enabled;

    if (enabled && !m_logFilePath.isEmpty() && !m_logFile) {
        m_logFile = new QFile(m_logFilePath);
        m_logFile->open(QIODevice::Append | QIODevice::Text);
    } 
    else if (!enabled && !m_logFile) {
        m_logFile->close();
        delete m_logFile;
        m_logFile = nullptr;
    }
}

void Logger::setLogFilePath(const QString &path)
{
    m_logFilePath = path;

    // Create directory if needed
    QDir dir = QFileInfo(path).absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

QString Logger::levelToString(Level level) const
{
    switch (level)
    {
        case Debug:    return "DEBUG";
        case Info:     return "INFO";
        case Warning:  return "WARN";
        case Error:    return "ERROR";
        case Critical: return "CRIT";
        default:       return "UNKNOWN";    
    }
}

QString Logger::formatLogEntry(Level level, const QString &message, const QString &source) const
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString levelStr= levelToString(level);

    if (source.isEmpty()) {
        return QString("[%1] [%2] %3").arg(timestamp, levelStr, message);
    }

    return QString("[%1] [%2] [%3] %4").arg(timestamp, levelStr, source, message);
} 

void Logger::writeToFile(const QString &entry)
{ 
    if (m_logFile && m_logFile->isOpen()) {
        QTextStream stream(m_logFile);
        stream << entry << "\n";
        m_logFile->flush();
    }
}

void Logger::trimLogs()
{
    while (m_logs.size() > m_maxLogEntries) {
        m_logs.removeLast();
    }
}
