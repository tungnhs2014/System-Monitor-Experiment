/*
 * ============================================
 * File: qml/components/Button.qml
 * Description: Reusable button component
 * ============================================
 */

import QtQuick 2.15

Rectangle {
    id: root

    // ==================== PROPERTIES ====================

    width: 100
    height: 28
    radius: 6

    property string text: "Button"
    property bool enabled: true
    
    // Colors
    property color buttonColor: "#2196F3"      // Default blue
    property color textColor: "#FFFFFF"        // White text
    property color pressedColor: Qt.darker(buttonColor, 1.2)
    property color disabledColor: "#4A5568"    // Gray when disabled
    
    // State
    property bool isPressed: false
    
    // ==================== SIGNALS ====================
    
    signal clicked()

    // ==================== BACKGROUND ====================
    
    color: {
        if (!root.enabled) return root.disabledColor
        if (root.isPressed) return root.pressedColor
        return root.buttonColor
    }

    // Smooth color transition
    Behavior on color {
        ColorAnimation {
            duration: 100
        }
    }

    // Border for depth
    border.width: 1
    border.color: Qt.darker(root.buttonColor, 1.3)
    
    // Opacity when disabled
    opacity: root.enabled ? 1.0 : 0.6

    // ==================== BUTTON TEXT ====================

    Text {
        id: buttonText
        anchors.centerIn: parent
        text: root.text
        font.family: "DejaVu Sans"
        font.pixelSize: 10
        font.bold: true
        color: root.textColor

        // RGB565 optimizations
        renderType: Text.NativeRendering
        antialiasing: false
        font.hintingPreference: Font.PreferFullHinting
    }

    // ==================== CLICK AREA ====================
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: root.enabled
        cursorShape: root.enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor

        onPressed: {
            root.isPressed = true
        }

        onReleased: {
            root.isPressed = false
        }
        
        onCanceled: {
            root.isPressed = false
        }
        
        onClicked: {
            root.clicked()
        }
    }
}
