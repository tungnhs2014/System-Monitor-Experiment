/*
 * ============================================
 * File: qml/views/pages/CpuDetail.qml
 * Description: CPU detail page with temp chart and core usage
 * ============================================
 */

import QtQuick 2.15
import "../components"

Rectangle {
    id: root

    // ==================== PROPERTIES ====================
    width: 320
    height: 240
    color: "#0F1419"

    // ==================== SIGNALS FOR NAVIGATION ====================
    signal backRequested()
    signal settingsRequested()
    signal navigationRequested(int index)

    // ==================== HEADER ====================
    DetailHeader {
        id: header
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        title: "CPU"

        onBackClicked: root.backRequested()
        onSettingsClicked: root.settingsRequested()
    }

    // ==================== CONTENT AREA ====================

    Item {
        id: contentArea
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            bottomMargin: 55
        }

        // ==================== TEMPERATURE CHART CARD ====================

        Rectangle {
            id: chartCard
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 8
            }
            width: 304
            height: 80
            radius: 6
            color: "#1E2539"
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.1)

            Column {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 4

                Row {
                    spacing: 4
                    Image {
                        source: "qrc:/assets/icons/thermometer.png"
                        width: 12
                        height: 12
                        sourceSize: Qt.size(12, 12)
                        smooth: false
                    }
                    Text {
                        text: "Temperature History"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 10
                        font.bold: true
                        color: "#FFFFFF"
                        renderType: Text.NativeRendering
                    }
                }

                LineChart {
                    width: parent.width
                    height: 50
                    dataPoints: systemInfo.tempHistory
                    maxValue: 100
                    lineColor: "#FF5722"
                }
            }   
        }

        // ==================== CORE USAGE BARS ====================

        Rectangle {
            id: coresCard
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: chartCard.bottom
                topMargin: 8
            }
            width: 304
            height: 86
            radius: 6
            color: "#1E2539"
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.1)

            Column {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 4

                Text {
                    text: "Per-Core Usage"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 10
                    font.bold: true
                    color: "#FFFFFF"
                    renderType: Text.NativeRendering
                }

                Repeater {
                    model: systemInfo.coreUsages

                    CoreUsageBar {
                        width: 288
                        coreIndex: index
                        usage: modelData
                    }
                }
            }
        }
    }

    // ==================== BOTTOM NAVIGATION ====================

    BottomNav {
        id: bottomNav
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        currentIndex: 1 

        onNavigationRequested: function(index) {
            root.navigationRequested(index)
        }
    }
}
