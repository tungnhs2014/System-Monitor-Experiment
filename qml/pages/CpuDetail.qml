/*
 * ============================================
 * File: qml/pages/CpuDetail.qml
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

    // ==================== MOCK DATA ====================
    property int mocCpuUsage: 57
    property int mockCpuTemp: 69
    property string mockCpuClock: "4.8G"

    // Temperature history (40 data points for smooth chart)
    property var mockTempHistory: [
        65, 67, 66, 68, 67, 69, 68, 70, 69, 71,
        70, 69, 68, 70, 71, 69, 72, 70, 71, 69,
        70, 72, 71, 70, 69, 71, 70, 72, 71, 73,
        72, 71, 70, 69, 71, 70, 69, 68, 70, 69
    ]

    // Per-core usage (4 cores)
    property var mockCoreUsages: [57, 62, 45,72]

    // ==================== HEADER ====================
    DetailHeader {
        id: header
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        title: "CPU"

        onBackClicked: {
            root.backRequested()
        }

        onSettingsClicked: {
            root.settingsRequested()
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
            color: "#1E2539"
            radius: 8
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.1)

            Column {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 4

                // Temperature label
                Text {
                    text: "TEMP" + root.mockCpuTemp + "°C"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 10
                    font.bold: true
                    color: root.mockCpuTemp > 80 ? "#FF9800" : "#4CAF50"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }

                // Line chart
                LineChart {
                    width: 288
                    height: 45
                    dataPoints: root.mockTempHistory
                    lineColor: "#4CAF50" // Green for temp
                    minValue: 60
                    maxValue: 80
                    smoothLine: true
                }

                // Load and clock info
                Text {
                    text: "Load " + root.mockCoreUsages + "% | Clock: " + root.mockCpuClock
                    font.family: "DejaVu Sans"
                    font.pixelSize: 10;
                    color: "#B0B8C8"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }
            }   
        }

        // ==================== CORE USAGE SECTION ====================

        Column {
            id: coreSection
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: chartCard.bottom
                topMargin: 8
            }
            width: 304
            spacing: 4

            // "Core Usage" label
            Text {
                text: "Core Usage:"
                font.family: "DejaVu Sans"
                font.pixelSize: 10
                font.bold: true
                color: "#B0B8C8"
                renderType: Text.NativeRendering
                antialiasing: false
                font.hintingPreference: Font.PreferFullHinting
            }

            // Core bars (4 cores)
            Repeater {
                model: root.mockCoreUsages
                
                CoreUsageBar {
                    width: 304
                    coreIndex: index
                    usage: modelData
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
        currentIndex: 1  // CPU tab (index 1)

        // Forward navigation signal to Main.qml
        onNavigationRequested: function(index) {
            root.navigationRequested(index)
        }
    }
}