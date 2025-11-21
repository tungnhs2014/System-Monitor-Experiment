/*
 * ============================================
 * File: qml/Main.qml
 * Description: Root window
 * ============================================
 */

import QtQuick 2.15
import QtQuick.Window 2.15
import "pages"

Window {
    id: root

    width: 320
    height: 240

    visible: true
    title: "ILI9341 System Monitor"

    // Frameless window for embedded
    flags: Qt.FramelessWindowHint

    // Load Dashboard page
    Dashboard {
        anchors.fill: parent
    }
}
