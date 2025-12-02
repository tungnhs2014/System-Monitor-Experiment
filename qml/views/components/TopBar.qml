/*
 * File: qml/views/omponents/TopBar.qml
 */

import QtQuick 2.15

Rectangle {
    id: root
    width: 320
    height: 30
    color: "#0A0E14"

    property string currentTime: "03:01"
    property string hostname: "raspberrypi"
    
    // ADD: Settings signal
    signal settingsClicked()

    // Time display
    Text {
        x: 16
        y: 11
        text: root.currentTime
        font.family: "DejaVu Sans"
        font.pixelSize: 14
        font.bold: true
        font.hintingPreference: Font.PreferFullHinting
        color: "#FFFFFF"
        renderType: Text.NativeRendering
        antialiasing: false
    }

    // Timer to update time
    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: root.currentTime = Qt.formatTime(new Date(), "hh:mm")
    }

    // Hostname display
    Text {
        anchors.centerIn: parent
        text: root.hostname
        font.family: "DejaVu Sans"
        font.pixelSize: 11
        font.bold: true
        font.hintingPreference: Font.PreferFullHinting
        color: "#B0B8C8"
        renderType: Text.NativeRendering
        antialiasing: false
    }

    Item {
        id: settingsButton
        anchors {
            right: parent.right
            rightMargin: 8
            verticalCenter: parent.verticalCenter
        }
        width: 44   
        height: 44

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
            onClicked: root.settingsClicked()  // Emit signal
            onPressed: settingsIcon.opacity = 0.6
            onReleased: settingsIcon.opacity = 1.0
            onCanceled: settingsIcon.opacity = 1.0
        }
    }
}
