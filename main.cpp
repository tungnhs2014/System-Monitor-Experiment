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

#include "SystemInfo.h"

int main(int argc, char *argv[])
{
    
    QGuiApplication app(argc, argv);

    // Application info
    app.setOrganizationName("ILI9341");
    app.setOrganizationDomain("ili9341_system");
    app.setApplicationName("System Monitor");
    app.setApplicationVersion("1.0.0");

#ifdef TARGET_PI
    qputenv("QT_QPA_PLATFORM", "linuxfb:fb=/dev/fb1");
    qputenv("QT_QPA_FB_FORCE_FULLSCREEN", "1");
    
    qputenv("QSG_RENDER_LOOP", "basic");
    
    // Touch input
    qputenv("QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS", "/dev/input/event0");

    qDebug() << "Running on Raspberry Pi - RGB565 mode";
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