/**
 * ============================================
 * File: src/utils/FileReader.cpp
 * Description: Implementation of file reading utilities
 * ============================================
 */

 #include "FileReader.h"
 #include <QFile>
 #include <QTextStream>
 #include <QDebug>

QString FileReader::readFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // qWarning() << "Failed to open file: " << path;
        return QString();
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return content;
}

QStringList FileReader::readLines(const QString &path)
{
    QString content = readFile(path);
    if (content.isEmpty()) {
        return QStringList();
    }

    return content.split('\n', Qt::SkipEmptyParts);
}

bool FileReader::fileExits(const QString &path)
{
    return QFile::exists(path);
}

QString FileReader::readFirstLine(const QString &path)
{
    QStringList lines = readLines(path);
    return lines.isEmpty() ? QString() : lines.first();
}

QMap<QString, QString> FileReader::parsekeyValue(const QString &content)
{
    QMap<QString, QString> result;
    QStringList lines = content.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        int colonPos = line.indexOf(':');

        if (colonPos > 0) {
            QString key = line.left(colonPos).trimmed();
            QString value = line.mid(colonPos + 1).trimmed();
            result[key] = value;
        }
    }

    return result;
}
