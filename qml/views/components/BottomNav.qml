/*
 * File: qml/views/components/BottomNav.qml
 */

import QtQuick 2.15

Rectangle {
    id: root
    width: 320
    height: 45
    color: "#0F1419"

    property int currentIndex: 0
    signal navigationRequested(int index)

    // Emit signal when index changes
    onCurrentIndexChanged: {
        navigationRequested(currentIndex)
    }

    Row {
        anchors.centerIn: parent
        spacing: 25

        NavButton {
            iconSource: "qrc:/assets/icons/home.png"
            active: root.currentIndex === 0
            onClicked: root.currentIndex = 0
        }

        NavButton {
            iconSource: "qrc:/assets/icons/cpu.png"
            active: root.currentIndex === 1
            onClicked: root.currentIndex = 1
        }

        NavButton {
            iconSource: "qrc:/assets/icons/memory.png"
            active: root.currentIndex === 2
            onClicked: root.currentIndex = 2
        }

        NavButton {
            iconSource: "qrc:/assets/icons/hard-drive.png"
            active: root.currentIndex === 3
            onClicked: root.currentIndex = 3
        }

        NavButton {
            iconSource: "qrc:/assets/icons/wifi.png"
            active: root.currentIndex === 4
            onClicked: root.currentIndex = 4
        }
    }

    component NavButton: Item {
        width: 44
        height: 44

        property string iconSource: ""
        property bool active: false
        signal clicked()

        Column {
            anchors.centerIn: parent
            spacing: 4

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                source: iconSource
                width: 24
                height: 24
                sourceSize: Qt.size(24, 24)
                opacity: active ? 1.0 : 0.5
                smooth: false 
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
