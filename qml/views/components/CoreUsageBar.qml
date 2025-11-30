/*
 * ============================================
 * File: qml/views/components/CoreUsageBar.qml
 * Description: Individual CPU core usage bar with label
 * ============================================
 */

import QtQuick 2.15

Item {
    id: root

    // ==================== PROPERTIES ====================

    width: 280
    height: 12  // Compact height for 4 bars

    property int coreIndex: 0           // Core number (0-3)
    property int usage: 0               // Usage percentage (0-100)

    // Auto-calculated properties
    property color barColor: _calculateColor()
    property string labelText: "C" + (coreIndex + 1)  + " " + usage + "%"

    // ==================== BACKGROUND BAR ====================

    Rectangle {
        id: backgroundBar
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
        width: parent.width - 60    // Leave space for label on right
        height: 8
        radius: 4
        color: Qt.rgba(1, 1, 1, 0.1)   // 10% white - gray background
    }

    // ==================== FILL BAR ====================
    
    Rectangle {
        id: fillBar
        anchors {
            left: backgroundBar.left
            verticalCenter: backgroundBar.verticalCenter
        }
        width: (backgroundBar.width * root.usage) / 100
        height: backgroundBar.height
        radius: backgroundBar.radius
        color: root.barColor

        // Smooth width animation when usage changes
        Behavior on width {
            NumberAnimation {
                duration: 300
                easing.type: Easing.OutCubic
            }
        }

        // Smooth color transition
        Behavior on color {
            ColorAnimation {
                duration: 200
            }
        }
    }

    // ==================== LABEL ====================

    Text {
        id: label
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        text: root.labelText
        font.family: "DejaVu Sans"
        font.pixelSize: 8
        font.bold: true
        color: root.barColor    // Match bar color

        // RGB565 optimizations
        renderType: Text.NativeRendering
        antialiasing: false
        font.hintingPreference: Font.PreferFullHinting

        // Smooth color transition
        Behavior on color {
            ColorAnimation {
                duration: 200
            }
        }
    }

    // ==================== COLOR LOGIC ====================
    function _calculateColor() {
        if (root.usage < 50) {
            return "#4CAF50";  // Green - low usage, good
        }
        else if (root.usage < 70) {
            return "#2196F3";  // Blue - medium usage, normal
        } 
        else {
            return "#FF9800";  // Orange - high usage, warning
        }
    }

    // Recalculate color when usage changes
    onUsageChanged: {
        barColor = _calculateColor();
    }

}