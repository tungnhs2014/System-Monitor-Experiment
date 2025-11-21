/*
 * File: qml/components/TopBar.qml
 */

import QtQuick 2.15

Rectangle {
    id: root
    width: 320
    height: 30
    color: "#0A0E14"

    property string currentTime: "03:01"
    property string hostname: "raspberrypi"

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

    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: root.currentTime = Qt.formatTime(new Date(), "hh:mm")
    }

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

    Rectangle {
        x: 285
        y: 5
        width: 20
        height: 20
        color: "transparent"

        Text {
            anchors.centerIn: parent
            text: "🖥️"
            font.pixelSize: 18
            font.hintingPreference: Font.PreferFullHinting
            renderType: Text.NativeRendering
            antialiasing: false
        }

        MouseArea {
            anchors.fill: parent
            onClicked: console.log("Settings clicked")
        }
    }
}
