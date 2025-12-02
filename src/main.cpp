/**
 * ============================================================================
 * File: src/main.cpp
 * Description: Application entry point
 * ============================================================================
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QFont>
#include <QFile>
#include <QDir>

#include "controller/SystemController.h"
#include "controller/NavigationController.h"

#include "common/Constants.h"
#include "common/Logger.h"

#ifdef PLATFORM_RASPBERRY_PI
/**
 * @brief Auto-detect XPT2046 touch input device
 */
QString findTouchDevice()
{
    for (int i = 0; i < 10; i++) {
        QString sysPath = QString("/sys/class/input/event%1/device/name").arg(i);

        QFile nameFile(sysPath);
        if (nameFile.open(QIODevice::ReadOnly | QIODevice:: Text)) {
            QString deviceName = QString::fromUtf8(nameFile.readAll()).trimmed();
            nameFile.close();

            qDebug() << "event" << i << ":" << deviceName;
            
            if (deviceName.contains("XPT2046", Qt::CaseInsensitive) ||
                deviceName.contains("ADS7846", Qt::CaseInsensitive) ||
                deviceName.contains("Touchscreen", Qt::CaseInsensitive)) {
                
                QString devicePath = QString("/dev/input/event%1").arg(i);
                qInfo() << "✓ Touch device found:" << devicePath << "-" << deviceName;
                return devicePath;
            }
        }
    }
    qWarning() << "Touch device not found, using fallback: /dev/input/event0";
    return "/dev/input/event0";
}

#endif

int main(int argc, char *argv[])
{
    // ==================== Platform Configuration ====================
#ifdef PLATFORM_RASPBERRY_PI
    // Raspberry Pi: Use linuxfb with ILI9341
    qputenv("QT_QPA_PLATFORM", "linuxfb:fb=/dev/fb1:size=320x240:mmSize=60x45:offset=0x0");
    
    QString touchDevice = findTouchDevice();
    QByteArray touchParams = QString("%1:rotate=0:invertx=0:inverty=0")
                            .arg(touchDevice).toUtf8();
    qputenv("QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS", touchParams);
    qputenv("QT_QPA_GENERIC_PLUGINS", QString("evdevtouch:%1").arg(touchDevice).toUtf8());
    
    qDebug() << "Platform: Raspberry Pi - RGB565 mode";
    qDebug() << "Touch device:" << touchDevice;
#else
    qDebug() << "Platform: Desktop";
#endif

    // ==================== Create Application ====================
    QGuiApplication app(argc, argv);

    // Application metadata
    app.setOrganizationName(App::Info::ORGANIZATION);
    app.setOrganizationDomain(App::Info::DOMAIN);
    app.setApplicationName(App::Info::NAME);
    app.setApplicationVersion(App::Info::VERSION);

    // ==================== Configure Font ====================
    QFont appFont;
    appFont.setFamily("DejaVu Sans");
    appFont.setHintingPreference(QFont::PreferFullHinting);
    app.setFont(appFont);

    // ==================== Initialize Logger ====================
    Logger::instance().setMaxLogEntries(100);
    LOG_INFO("Application starting...");

    // ==================== Create QML Engine ====================
    QQmlApplicationEngine engine;

    // ==================== Create Controllers ====================
    SystemController* systemController = new SystemController(&app);
    NavigationController* navController = new NavigationController(&app);

    // ==================== Register Context Properties ====================
    engine.rootContext()->setContextProperty("systemInfo", systemController);
    engine.rootContext()->setContextProperty("navigation", navController);

    // Logger is only available in QML when LOG_LEVEL > 0 (Debug/Development builds)
    // In Release builds (LOG_LEVEL=0), Logger is a stub without QObject inheritance
#if LOG_LEVEL > LOG_LEVEL_OFF
    engine.rootContext()->setContextProperty("logger", &Logger::instance());
#endif

    // ==================== Configure QML Debug Logging ====================
    // Enable debug logs in Debug builds, disable in Release for performance
#ifdef QT_NO_DEBUG
    // Release build - disable all QML console.log()
    engine.rootContext()->setContextProperty("QML_DEBUG_ENABLED", false);
#else
    // Debug build - enable QML console.log()
    engine.rootContext()->setContextProperty("QML_DEBUG_ENABLED", true);
#endif
    
    // ==================== Load QML ====================
    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject* obj, const QUrl& objUrl) {
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

    // ==================== Log Startup Complete ====================
    LOG_INFO("Application started successfully");
    
    qDebug() << "==========================================";
    qDebug() << App::Info::NAME << "v" << App::Info::VERSION;
    qDebug() << "==========================================";

    return app.exec();
}