/*
 * ============================================
 * File: main.cpp
 * Description: Main entry point - can load different QML files
 * ============================================
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QScreen>
#include <QDebug>

int main(int argc, char *argv[])
{
    // Set attributes before QGuiApplication
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    QGuiApplication app(argc, argv);
    
    // Application metadata
    app.setOrganizationName("ILI9341");
    app.setOrganizationDomain("ili9341.local");
    app.setApplicationName("System Monitor");
    
    // ==================== QML ENGINE ====================
    
    QQmlApplicationEngine engine;

    
    // ==================== DETERMINE WHICH QML TO LOAD ====================
    
    // OPTION 1: Use command-line argument
    // Run: ./ili9341-system memory
    //      ./ili9341-system cpu
    //      ./ili9341-system storage
    
    QString qmlFile = "qrc:/qml/Main.qml";  // Default
    
    if (argc > 1) {
        QString arg = QString(argv[1]).toLower();
        
        if (arg == "memory") {
            qmlFile = "qrc:/qml/Main_MemoryDetail.qml";
            qDebug() << "Loading Memory Detail page...";
        }
        else if (arg == "cpu") {
            qmlFile = "qrc:/qml/Main_CpuDetail.qml";
            qDebug() << "Loading CPU Detail page...";
        }
        else if (arg == "storage") {
            qmlFile = "qrc:/qml/Main_StorageDetail.qml";
            qDebug() << "Loading Storage Detail page...";
        }
        else if (arg == "dashboard") {
            qmlFile = "qrc:/qml/Main_Dashboard.qml";
            qDebug() << "Loading Dashboard page...";
        }
        else {
            qDebug() << "Unknown argument:" << arg;
            qDebug() << "Usage: ./ili9341-system [memory|cpu|storage|dashboard]";
        }
    }
    
    // OPTION 2: Hardcode for testing (comment out the above, use this)
    /*
    // Uncomment ONE of these:
    // qmlFile = "qrc:/qml/Main_MemoryDetail.qml";
    // qmlFile = "qrc:/qml/Main_CpuDetail.qml";
    // qmlFile = "qrc:/qml/Main_StorageDetail.qml";
    // qmlFile = "qrc:/qml/Main_Dashboard.qml";
    */
    
    // ==================== LOAD QML ====================
    
    qDebug() << "Loading QML file:" << qmlFile;
    
    const QUrl url(qmlFile);
    
    // Error handling
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            qCritical() << "Failed to load QML file:" << url;
            QCoreApplication::exit(-1);
        } else {
            qDebug() << "QML loaded successfully!";
        }
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    // ==================== SCREEN INFO (Debug) ====================
    
    QScreen *screen = app.primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        qDebug() << "Screen geometry:" << screenGeometry;
        qDebug() << "Screen size:" << screenGeometry.width() << "x" << screenGeometry.height();
        qDebug() << "Physical DPI:" << screen->physicalDotsPerInch();
    }
    
    // ==================== START EVENT LOOP ====================
    
    return app.exec();
}

/*
 * ============================================
 * HƯỚNG DẪN SỬ DỤNG:
 * ============================================
 *
 * CÁCH 1: Command-line argument (Linh hoạt nhất)
 * ------------------------------------------------
 * Không cần sửa code, chỉ chạy với tham số:
 *
 * ./ili9341-system               # Chạy Main.qml (default)
 * ./ili9341-system memory        # Chạy Memory Detail
 * ./ili9341-system cpu           # Chạy CPU Detail
 * ./ili9341-system storage       # Chạy Storage Detail
 * ./ili9341-system dashboard     # Chạy Dashboard
 *
 * CÁCH 2: Hardcode (Nhanh cho testing)
 * -------------------------------------
 * Comment out phần "OPTION 1", uncomment "OPTION 2":
 *
 * qmlFile = "qrc:/qml/Main_MemoryDetail.qml";
 *
 * Sau đó recompile:
 * cd build && make && ./ili9341-system
 *
 * ============================================
 * LƯU Ý:
 * ============================================
 *
 * 1. Các file Main_*.qml phải có trong qml.qrc
 * 2. Sau khi sửa qml.qrc, phải rebuild:
 *    cd build && cmake .. && make
 * 3. Command-line argument chỉ work nếu code OPTION 1 active
 *
 * ============================================
 */
