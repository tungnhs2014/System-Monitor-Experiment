/*
 * ============================================
 * File: qml/components/DetailHeader.qml
 * Description: Reusable header for detail pages
 * ============================================
 */

import QtQuick 2.15

Rectangle {
    id: root

    // ==================== PROPERTIES ====================

    // Size properties
    width: 320
    height: 30
   
    // Customizable properties
    property string title: "DETAIL"                   // Page title text
    property color backgroundColor: "#0F1419"       // Header background
    property color textColor: "#FFFFFF"             // Text and iconn color
    property int fontSize: 12                         // Title font size
    property bool showBackButton: true                // Show/hide back button
    property bool showSettingsIcon: true              // Show/hide setting icon 

    // ==================== SIGNALS ====================

    signal backClicked()        // Emitted when back button clicked
    signal settingsClicked()    // Emitted when settings icon clicked 

    // ==================== BACKGROUND ====================

    color: backgroundColor

    // Bottom border line (divider)
    Rectangle {
        id: bottomBorder
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 1
        color: Qt.rgba(1, 1, 1, 0.1 )   // rgba(255, 255, 255, 0.1)
    }

    // ==================== BACK BUTTON ====================

    Item {
        id: backbutton
        anchors {
            left:parent.left
            leftMargin: 8
            verticalCenter: parent.verticalCenter
        }
        width: 44
        height: 44
        visible: root.showBackButton

        // Back arrow text
        Text {
            id: backArrow
            anchors.centerIn: parent
            text: "←"
            font.family: "DejaVu Sans"
            font.pixelSize: root.fontSize + 2
            font.bold: true
            color: root.textColor

            // RGB565 optimization
            renderType: Text.NativeRendering
            antialiasing: false
            font.hintingPreference: Font.PreferFullHinting
        }

        // Click area
        MouseArea {
            id: backMouseArea
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor

            onClicked: {
                root.backClicked()
            }

            // Visual feedback on press
            onPressed: {
                backArrow.color = Qt.rgba(1, 1, 1, 0.6)
            }
            
            onReleased: {
                backArrow.color = root.textColor
            }
            
            onCanceled: {
                backArrow.color = root.textColor
            }
        }
    }

    // ==================== TITLE TEXT ====================

    Text {
        id: tilteText
        anchors.centerIn: parent
        text: root.title
        font.family: "DejaVu Sans"
        font.pixelSize: root.fontSize
        font.bold: true
        color: root.textColor

        // RGB565 optimization for clean rendering
        renderType: Text.NativeRendering
        antialiasing: false
        font.hintingPreference: Font.PreferFullHinting
    }

    // ==================== SETTINGS ICON ====================

    Item {
        id: settingsButton
        anchors {
            right: parent.right
            rightMargin: 8
            verticalCenter: parent.verticalCenter
        }
        width: 30
        height: 30
        visible: root.showSettingsIcon

        // Settings gear icon
        Text {
            id: settingsIcon
            anchors.centerIn: parent
            text: "⚙"
            font.family: "DejaVu Sans"
            font.pixelSize: 14
            color: root.textColor

            // RGB565 optimizations
            renderType: Text.NativeRendering
            antialiasing: false
            font.hintingPreference: Font.PreferFullHinting
        }

        // Clicked area
        MouseArea {
            id: settingsMouseArea
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor

            onClicked: {
                root.settingsClicked()
            }

            // Visual feedback on press
            onPressed: {
                settingsIcon.color = Qt.rgba(1, 1, 1, 0.6)
            }

            onReleased: {
                settingsIcon.color = root.textColor
            }

            onCanceled: {
                settingsIcon.color = root.textColor
            }
        }
    }
}