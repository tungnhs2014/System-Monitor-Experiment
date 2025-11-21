/*
 * File: qml/components/BottomNav.qml
 */

import QtQuick 2.15

Rectangle {
    id: root
    width: 320
    height: 45
    color: "#0A0E14"

    property int currentIndex: 0

    Row {
        anchors.centerIn: parent
        spacing: 15 

        NavButton {
            icon: "🏠"
            active: root.currentIndex === 0
            onClicked: root.currentIndex = 0
        }

        NavButton {
            icon: "📊"
            active: root.currentIndex === 1
            onClicked: root.currentIndex = 1
        }

        NavButton {
            icon: "💾"
            active: root.currentIndex === 2
            onClicked: root.currentIndex = 2
        }

        NavButton {
            icon: "🌐"
            active: root.currentIndex === 3
            onClicked: root.currentIndex = 3
        }

        NavButton {
            icon: "🖥️"
            active: root.currentIndex === 4
            onClicked: root.currentIndex = 4
        }
    }

    component NavButton: Item {
        width: 32
        height: 32

        property string icon: ""
        property bool active: false
        signal clicked()

        Column {
            anchors.centerIn: parent
            spacing: 4

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: icon
                font.pixelSize: 24
                font.hintingPreference: Font.PreferFullHinting
                opacity: active ? 1.0 : 0.5
                renderType: Text.NativeRendering
                antialiasing: false

                Behavior on opacity {
                    NumberAnimation { duration: 200 }
                }
            }

            Rectangle {
                width: 20
                height: 2
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#2196F3"
                visible: active
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: parent.clicked()
        }
    }
}
