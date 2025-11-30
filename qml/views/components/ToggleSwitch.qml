/*
 * ============================================
 * File: qml/views/components/ToggleSwitch.qml
 * Description: ON/OFF toggle switch component
 * ============================================
 */

import QtQuick 2.15

Item {
    id: root

    // ==================== PROPERTIES ====================
    
    width: 40
    height: 20

    property bool checked: false
    property bool enabled: true

    // Colors
    property color onColor: "#2196F3"          // Blue when ON
    property color offColor: "#4A5568"         // Gray when OFF
    property color thumbColor: "#FFFFFF"       // White circle
    property color disabledColor: "#2D3748"    // Dark gray when disabled 

    // ==================== SIGNALS ====================
    
    signal toggled(bool state)

    // ==================== BACKGROUND TRACK ====================

    Rectangle {
        id: track
        anchors.fill: parent
        radius: height / 2  // Fully rounded
        color: {
            if (!root.enabled) return root.disabledColor
            return root.checked ? root.onColor : root.offColor
        }

        // Smooth color transition
        Behavior on color {
            ColorAnimation {
                duration: 200
            }
        }

        // Opacity when disabled
        opacity: root.enabled ? 1.0 : 0.5
    }

    // ==================== THUMB (CIRCLE) ====================
    
    Rectangle {
        id: thumb
        width: 16
        height: 16
        radius: width / 2
        color: root.thumbColor
        anchors.verticalCenter: parent.verticalCenter

        // Position based on checked state
        x: root.checked ? (root.width - width - 2) : 2

        // Smooth slide animation
        Behavior on x {
            NumberAnimation {
                duration: 200
                easing.type: Easing.InOutCubic
            }
        }
    }

    // ==================== CLICK AREA ====================

    MouseArea {
        anchors.fill: parent
        enabled: root.enabled
        cursorShape: root.enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor

        onClicked: {
            root.checked = !root.checked
            root.toggled(root.checked)
        }

        // Visual feedback on press (optional)
        onPressed: {
            track.opacity = 0.8
        }
        
        onReleased: {
            track.opacity = root.enabled ? 1.0 : 0.5
        }
        
        onCanceled: {
            track.opacity = root.enabled ? 1.0 : 0.5
        }
    }
}
