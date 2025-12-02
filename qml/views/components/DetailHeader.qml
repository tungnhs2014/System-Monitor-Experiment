/*
 * ============================================
 * File: qml/views/components/DetailHeader.qml
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
    property string title: "DETAIL"
    property color backgroundColor: "#0F1419"
    property color textColor: "#FFFFFF"
    property int fontSize: 12
    property bool showBackButton: true
    property bool showSettingsIcon: true

    // ==================== SIGNALS ====================

    signal backClicked()
    signal settingsClicked()

    // ==================== BACKGROUND ====================

    color: backgroundColor

    // Bottom border line
    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 1
        color: Qt.rgba(1, 1, 1, 0.1)
    }

    // ==================== BACK BUTTON ====================

    Item {
        id: backButton
        anchors {
            left: parent.left
            leftMargin: 8
            verticalCenter: parent.verticalCenter
        }
        width: 44
        height: 44
        visible: root.showBackButton

        // Back arrow icon (PNG)
        Image {
            id: backIcon
            anchors.centerIn: parent
            source: "qrc:/assets/icons/arrow-left.png"
            width: 20
            height: 20
            sourceSize: Qt.size(20, 20)
            smooth: false
            opacity: 1.0
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.backClicked()
            onPressed: backIcon.opacity = 0.6
            onReleased: backIcon.opacity = 1.0
            onCanceled: backIcon.opacity = 1.0
        }
    }

    // ==================== TITLE TEXT ====================

    Text {
        id: titleText
        anchors.centerIn: parent
        text: root.title
        font.family: "DejaVu Sans"
        font.pixelSize: root.fontSize
        font.bold: true
        color: root.textColor
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
        width: 44   // ✅ Touch target 44x44
        height: 44  // ✅ Touch target 44x44
        visible: root.showSettingsIcon

        // Settings gear icon (PNG)
        Image {
            id: settingsIcon
            anchors.centerIn: parent
            source: "qrc:/assets/icons/settings.png"
            width: 20
            height: 20
            sourceSize: Qt.size(20, 20)
            smooth: false
            opacity: 1.0
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.settingsClicked()
            onPressed: settingsIcon.opacity = 0.6
            onReleased: settingsIcon.opacity = 1.0
            onCanceled: settingsIcon.opacity = 1.0
        }
    }
}