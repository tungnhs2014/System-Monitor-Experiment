/*
 * File: qml/views/components/CardContainer.qml
 */

import QtQuick 2.15

Rectangle {
    width: 100
    height: 75
    color: "#1C3A5C" 
    radius: 8
    border.width: 1
    border.color: Qt.rgba(1, 1, 1, 0.15)

    default property alias content: contentArea.data

    Item {
        id: contentArea
        anchors.fill: parent
        anchors.margins: 8
    }
}
