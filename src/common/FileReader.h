/**
 * ============================================
 * File: src/common/FileReader.h
 * Description: Utility class for reading system files (/proc, /sys)
 * ============================================
 */

#ifndef FILEREADER_H
#define FILEREADER_H

#include <QString>
#include <QStringList>
#include <QMap>

/**
 * @class FileReader
 * @brief Static utility class for reading Linux system files
 * 
 * Provides optimized methods for reading:
 * - /proc filesystem
 * - /sys filesystem
 * - Configuration files
 */

class FileReader
{
public:
    /**
     * @brief Read entire file content
     * @param path File path
     * @return File content or empty string on error
     */
    static QString readAll(const QString& path);
    
    /**
     * @brief Read first line of file
     * @param path File path
     * @return First line or empty string on error
     */
    static QString readFirstLine(const QString& path);
    
    /**
     * @brief Read all lines of file
     * @param path File path
     * @return List of lines
     */
    static QStringList readLines(const QString& path);
    
    /**
     * @brief Read file and parse as key:value pairs
     * @param path File path
     * @param delimiter Separator between key and value (default: ":")
     * @return Map of key-value pairs
     */
    static QMap<QString, QString> readKeyValueFile(
        const QString& path, 
        const QString& delimiter = ":"
    );
    
    /**
     * @brief Read integer value from file
     * @param path File path
     * @param defaultValue Default value if read fails
     * @return Integer value
     */
    static int readInt(const QString& path, int defaultValue = 0);
    
    /**
     * @brief Read double value from file
     * @param path File path
     * @param defaultValue Default value if read fails
     * @return Double value
     */
    static double readDouble(const QString& path, double defaultValue = 0.0);
    
    /**
     * @brief Execute command and get output
     * @param command Command to execute
     * @return Command output
     */
    static QString executeCommand(const QString& command);
    
    /**
     * @brief Check if file exists and is readable
     * @param path File path
     * @return true if file exists and is readable
     */
    static bool fileExists(const QString& path);
    
    /**
     * @brief Get network interface list
     * @return List of interface names
     */
    static QStringList getNetworkInterfaces();
    
    /**
     * @brief Get primary network interface
     * @return Primary interface name (eth0, wlan0, etc.)
     */
    static QString getPrimaryInterface();

private:
    // Prevent instantiation
    FileReader() = default;
    ~FileReader() = default;
};

#endif // FILEREADER_H
