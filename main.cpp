/**
 * ============================================
 * File: main.cpp
 * Description: Application entry point
 * ============================================
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQmlContext>
#include <QtDebug>
#include <QFont>
#include <QDir>
#include <QFile>

#include "SystemInfo.h"

#ifdef PLATFORM_RASPBERRY_PI
/**
 * Auto-detect XPT2046 touch input device by checking device name in sysfs
 * This is the professional and accurate method to find the correct input device
 */
QString findTouchDevice() {
    // Search through all input event devices
    for (int i = 0; i < 10; i++) {
        QString eventNum = QString::number(i);
        QString sysPath = QString("/sys/class/input/event%1/device/name").arg(i);

        QFile nameFile(sysPath);
        if (nameFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString deviceName = QString::fromUtf8(nameFile.readAll()).trimmed();
            nameFile.close();

            qDebug() << "event" << i << ":" << deviceName;

            // Check if this is our XPT2046 touchscreen
            if (deviceName.contains("XPT2046", Qt::CaseInsensitive) ||
                deviceName.contains("ADS7846", Qt::CaseInsensitive) ||
                deviceName.contains("Touchscreen", Qt::CaseInsensitive)) {

                QString devicePath = QString("/dev/input/event%1").arg(i);
                qInfo() << "✓ Touch device found:" << devicePath << "-" << deviceName;
                return devicePath;
            }
        }
    }

    // Fallback: if not found, use event0 (first input device)
    qWarning() << "⚠ XPT2046 touch device not found, using fallback: /dev/input/event0";
    qWarning() << "→ Check if ili9341-touch kernel module is loaded: lsmod | grep ili9341_touch";
    return "/dev/input/event0";
}
#endif

int main(int argc, char *argv[])
{
    
    QGuiApplication app(argc, argv);

    // Application info
    app.setOrganizationName("ILI9341");
    app.setOrganizationDomain("ili9341_system");
    app.setApplicationName("System Monitor");
    app.setApplicationVersion("1.0.0");

#ifdef PLATFORM_RASPBERRY_PI
    qputenv("QT_QPA_PLATFORM", "linuxfb:fb=/dev/fb1:size=320x240:mmSize=60x45:offset=0x0");
    
    QString touchDevice = findTouchDevice();
    
    QByteArray touchParams = QString("%1:rotate=0:invertx=0:inverty=0")
                            .arg(touchDevice).toUtf8();
    qputenv("QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS", touchParams);
    
    // Force evdevtouch plugin
    qputenv("QT_QPA_GENERIC_PLUGINS", QString("evdevtouch:%1").arg(touchDevice).toUtf8());

    qDebug() << "Running on Raspberry Pi - RGB565 mode";
    qDebug() << "Touch device configured:" << touchDevice;
#else
    qDebug() << "Running on Desktop";
#endif

    QFont appFont;
    appFont.setFamily("DejaVu Sans");  // Guaranteed in Yocto
    appFont.setHintingPreference(QFont::PreferFullHinting);
    app.setFont(appFont);

    // Create QML engine
    QQmlApplicationEngine engine;

    // Create SystemInfo instance
    SystemInfo *systemInfo = new SystemInfo(&app);

    // Register SystemInfo
    engine.rootContext()->setContextProperty("systemInfo", systemInfo);
    qmlRegisterType<SystemInfo>("com.ili9341.system", 1, 0, "SystemInfo");

    // Load main QML
    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            qCritical() << "Failed to load QML";
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "No root objects loaded";
        return -1;
    }

    qDebug() << "Application started successfully - RGB mode enabled";
    
    return app.exec();
}