/*
 * File: qml/components/ProgressBar.qml
 */

import QtQuick 2.15

Rectangle {
    id: root

    property real value: 0
    property color fillColor: "#2196F3"
    property color backgroundColor: "#2A3A4A"

    radius: height / 2
    color: backgroundColor

    Rectangle {
        width: Math.max(0, Math.min(parent.width, parent.width * (root.value / 100)))
        height: parent.height
        radius: parent.radius
        color: root.fillColor

        Behavior on width {
            NumberAnimation { duration: 300 }
        }
    }
}
