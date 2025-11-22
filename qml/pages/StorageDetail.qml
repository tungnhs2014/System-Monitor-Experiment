/*
 * ============================================
 * File: qml/pages/StorageDetail.qml
 * Description: Storage detail with partitions and I/O stats
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

    // ==================== MOCK DATA ====================

    // Root partition
    property string mockRootName: "Root Partition"
    property string mockRootTotal: "85.1 GB"
    property int mockRootUsage: 95
    property string mockRootUsed: "80.8 GB"
    property string mockRootFree: "4.3 GB"

    // Swap partition
    property string mockSwapName: "Swap Partition"
    property string mockSwapTotal: "100 MB"
    property int mockSwapUsage: 0
    property string mockSwapUsed: "0 MB"
    property string mockSwapFree: "100 MB"

    // I/O statistics
    property string mockReadSpeed: "0.0 MB/s"
    property string mockWriteSpeed: "0.0 MB/s"
    property var mockIoHistory: [
        10, 15, 12, 18, 14, 20, 16, 22, 18, 25,
        20, 23, 19, 24, 21, 26, 22, 28, 24, 30,
        25, 29, 26, 31, 27, 32, 28, 30, 26, 28
    ]

    // ==================== HEADER ====================

    DetailHeader {
        id: header
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        title: "STORAGE"

        onBackClicked: {
            console.log("Back to Dashboard")
        }
        
        onSettingsClicked: {
            console.log("Open Settings")
        }
    }

    // ==================== CONTENT AREA ====================

    Item {
        id: contentArea
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: bottomNav.top
        }

        Column {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 8
            }
            spacing: 8

            // ==================== ROOT PARTITION ====================

            PartitionBar {
                width: 304
                partitionName: root.mockRootName
                totalSize: root.mockRootTotal
                usage: root.mockRootUsage
                usedSize: root.mockRootUsed
                freeSize: root.mockRootFree
            }

            // ==================== SWAP PARTITION ====================

            PartitionBar {
                width: 304
                partitionName: root.mockSwapName
                totalSize: root.mockSwapTotal
                usage: root.mockSwapUsage
                usedSize: root.mockSwapUsed
                freeSize: root.mockSwapFree
            }

            // ==================== I/O STATISTICS CARD ====================

            Rectangle {
                width: 304
                height: 40
                color: "#1E2539"
                radius: 8
                border.width: 1
                border.color: Qt.rgba(1, 1, 1, 0.1)

                Column {
                    anchors.fill: parent
                    anchors.margins: 2
                    spacing: 2

                    // Title
                    Text {
                        text: "I/O Statistics"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 8
                        font.bold: true
                        color: "#B0B8C8"
                        renderType: Text.NativeRendering
                        antialiasing: false
                        font.hintingPreference: Font.PreferFullHinting
                    }

                    // Mini chart
                    LineChart {
                        width: 288
                        height: 20
                        dataPoints: root.mockIoHistory
                        lineColor: "#FFEB3B"  // Yellow for I/O
                        smoothLine: true
                    }
                }
            }

            // ==================== I/O SPEEDS ====================

            Row {
                width: 304
                spacing: 10

                Text {
                    text: "Read: " + root.mockReadSpeed
                    font.family: "DejaVu Sans"
                    font.pixelSize: 8
                    color: "#B0B8C8"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }

                Text {
                    text: "Write: " + root.mockWriteSpeed
                    font.family: "DejaVu Sans"
                    font.pixelSize: 8
                    color: "#B0B8C8"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
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
        currentIndex: 0
    }

}