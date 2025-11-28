/**
 * ============================================
 * File: src/common/FileReader.cpp
 * Description: FileReader implementationes
 * ============================================
 */

 #include "FileReader.h"
 #include <QFile>
 #include <QTextStream>
 #include <QProcess>
 #include <QDir>
 #include <QFileInfo>

QString FileReader::readAll(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QTextStream stream(&file);
    QString content = stream.readAll();
    file.close();

    return content;
}

QString FileReader::readFirstLine(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }

    QTextStream stream(&file);
    QString line = stream.readLine();
    file.close();

    return line.trimmed();
}

QStringList FileReader::readLines(const QString &path)
{
    QStringList lines;
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return lines;
    }

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        lines.append(stream.readLine());
    }

    file.close();
    return lines;
}

QMap<QString, QString> FileReader::readKeyValueFile(const QString &path, const QString &delimiter)
{
    QMap<QString, QString> result;
    QStringList lines= readLines(path);

    for (const QString& line : lines) {
        int pos = line.indexOf(delimiter);
        if (pos > 0) {
            QString key = line.left(pos).trimmed();
            QString value = line.mid(pos + delimiter.length()).trimmed();
            result[key] = value; 
        }
    }

    return result;
}

int FileReader::readInt(const QString &path, int defaultValue)
{
    QString content = readFirstLine(path);
    if (content.isEmpty()) {
        return defaultValue;
    }

    bool ok;
    int value = content.toInt(&ok);

    return ok ? value : defaultValue;
}

double FileReader::readDouble(const QString &path, double defaultValue)
{
    QString content = readFirstLine(path);
    if (content.isEmpty()) {
        return defaultValue;
    }

    bool ok;
    double value = content.toDouble(&ok);
    return ok ? value : defaultValue;
}

QString FileReader::executeCommand(const QString &command)
{
    QProcess process;
    process.start("sh", QStringList() << "-c" << command);
    process.waitForFinished(3000); // 3 scecond timeout

    return QString::fromUtf8(process.readAllStandardOutput()).trimmed());
}

bool FileReader::fileExists(const QString &path)
{
    QFileInfo fileInfo(path);
    return fileInfo.exists() && fileInfo.isReadable();
}

QStringList FileReader::getNetWorkInterface()
{
    QStringList interfaces;
    QDir netDir("/sys/class/net");

    if (netDir.exists()) {
        QStringList entries = netDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString& entry : entries) {
            // Skip loopback
            if (entry != "lo") {
                interfaces.append(entry);
            }
        }
    }

    return interfaces;
}

QString FileReader::getPrimaryInterface()
{
    // Priority: eth0 > wlan0 > any other
    QStringList interfaces = getNetWorkInterface();

    // Check for enthernet first
    for (const QString& iface: interfaces) {
        if (iface.startsWith("eth")) {
            return iface;
        }
    }

    // Check for wireless
    for (const QString& iface : interfaces) {
        if (iface.startsWith("wlan")) {
            return iface;
        }
    }
    
    // Return first available
    if (!interfaces.isEmpty()) {
        return interfaces.first();
    }
    
    return "eth0"; // Default fallback
}
