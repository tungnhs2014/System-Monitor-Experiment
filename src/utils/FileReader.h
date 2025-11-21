/**
 * ============================================
 * File: src/utils/FileReader.h
 * Description: Utility for reading /proc and /sys files
 * ============================================
 */

#ifndef FILEREADER_H
#define FILEREADER_H

#include <QString>
#include <QStringList>

class FileReader
{
public:
    // Read entire file as string
    static QString readFile(const QString &path);

    // Read file as list of line
    static QStringList readLines(const QString &path);

    // Check if file exits
    static bool fileExits(const QString &path);

    // Read single line from file
    static QString readFirstLine(const QString &path);

    // Parse key-value pairs (format: "Key" : "Value")
    static QMap<QString, QString> parsekeyValue(const QString &content);
};

#endif // FILEREADER_H
