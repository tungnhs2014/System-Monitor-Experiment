/*
 * ============================================
 * File: qml/pages/NetworkDetail.qml
 * Description: Network detail with dual-line chart and stats
 * ============================================
 *
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

    property string mockInterface: "eth0"
    property string mockIpAddress: "192.168.40.31"
    property string mockMacAddress: "b8:27:eb:xx:xx:xx"

    // Current speeds
    property string mockUpSpeed: "0.0 Bps"
    property string mockDownSpeed: "0.0 Bps"

    // Historical data (30 points for 60 seconds at 2s interval)
    property var mockUpHistory: [
        5, 8, 6, 10, 7, 9, 8, 11, 9, 12,
        10, 13, 11, 14, 12, 15, 13, 16, 14, 17,
        15, 14, 13, 12, 11, 10, 9, 8, 7, 6
    ]

    property var mockDownHistory: [
        15, 20, 18, 25, 22, 27, 24, 30, 26, 32,
        28, 35, 30, 38, 33, 40, 35, 42, 37, 45,
        40, 38, 35, 32, 30, 28, 25, 22, 20, 18
    ]

    // Network stats
    property string mockPacketRate: "0 pps"
    property int mockActiveConnections: 7

    // ==================== DEBUG ====================
    Component.onCompleted: {
        console.log("=== NetworkDetail DEBUG ===")
        console.log("Interface:", systemInfo.networkInterface)
        console.log("IP:", systemInfo.ipAddress)
        console.log("MAC:", systemInfo.macAddress)
        console.log("Upload history length:", systemInfo.netUpHistory.length)
        console.log("Download history length:", systemInfo.netDownHistory.length)
    }

    Connections {
        target: systemInfo
        function onNetUpHistoryChanged() {
            console.log("Upload history updated, length:", systemInfo.netUpHistory.length)
        }
        function onNetDownHistoryChanged() {
            console.log("Download history updated, length:", systemInfo.netDownHistory.length)
        }
    }

    // ==================== HEADER ==================== 
    DetailHeader {
        id: header
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        title: "NETWORK"

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

        // ==================== NETWORK INFO CARD ====================

        Rectangle {
            id: infoCard
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 8
            }
            width: 304
            height: 30
            color: "#1E2539"
            radius: 8
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.1)

            Row {
                anchors {
                    left: parent.left
                    leftMargin: 8
                    verticalCenter: parent.verticalCenter
                }
                spacing: 16

                // Interface info
                Column {
                    spacing: 2

                    Text {
                        text: "Interface: " + systemInfo.networkInterface
                        font.family: "DejaVu Sans"
                        color: "#FFFFFF"
                        renderType: Text.NativeRendering
                        antialiasing: false
                        font.hintingPreference: Font.PreferFullHinting
                    }

                    Text {
                        text: "MAC: " + systemInfo.macAddress
                        font.family: "DejaVu Sans"
                        font.pixelSize: 7
                        color: "#B0B8C8"
                        renderType: Text.NativeRendering
                        antialiasing: false
                        font.hintingPreference: Font.PreferFullHinting
                    }
                }

                // IP address
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "IP: " + systemInfo.ipAddress
                    font.family: "Dejavu Sans"
                    font.pixelSize: 9
                    color: "#FFFFFF"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }
            }
        }

        // ==================== TRAFFIC CHART CARD ====================

        Rectangle {
            id: chartCard
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: infoCard.bottom
                topMargin: 8
            }
            width: 304
            height: 60
            color: "#1E2539"
            radius: 8
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.1)

            Column {
                anchors.fill: parent
                anchors.margins: 6
                spacing: 2

                // Title
                Text {
                    text: "Network Traffic"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 8
                    font.bold: true
                    color: "#B0B8C8"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }

                // Dual-line chart
                Item {
                    width: 292
                    height: 40

                    // Download line (behind, orange)
                    LineChart {
                        anchors.fill: parent
                        dataPoints: systemInfo.netDownHistory
                        lineColor: "#FF9800"  // Orange for download
                        smoothLine: true
                        lineWidth: 2
                    }

                    // Upload line (behind, cyan)
                    LineChart {
                        anchors.fill: parent
                        dataPoints: systemInfo.netUpHistory
                        lineColor: "#00BCD4"  // Orange for download
                        smoothLine: true
                        lineWidth: 2
                    }

                    // Legend
                    Row {
                        anchors {
                            right: parent.right
                            top: parent.top
                            topMargin: 2
                        }
                        spacing: 8

                        // Upload legend
                        Row {
                            spacing: 3

                            Rectangle {
                                width: 12
                                height: 2
                                color: "#00BCD4"
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Text {
                                text: "↑Up"
                                font.family: "DejaVu Sans"
                                font.pixelSize: 7
                                color: "#00BCD4"
                                renderType: Text.NativeRendering
                                antialiasing: false
                                font.hintingPreference: Font.PreferFullHinting
                            }
                        }

                        // Download legend
                        Row {
                            spacing: 3

                            Rectangle {
                                width: 12
                                height: 2
                                color: "#FF9800"
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Text {
                                text: "↓Down"
                                font.family: "DejaVu Sans"
                                font.pixelSize: 7
                                color: "#FF9800"
                                renderType: Text.NativeRendering
                                antialiasing: false
                                font.hintingPreference: Font.PreferFullHinting
                            }
                        }
                    }
                }
            }
        }

        // ==================== NETWORK STATS ====================

        Column {
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: chartCard.bottom
                topMargin: 8
            }
            width: 304
            spacing: 4

            // Current speeds
            Row {
                spacing: 10

                Text {
                    text: "↑ Up: " + systemInfo.netUpSpeed
                    font.family: "DejaVu Sans"
                    font.pixelSize: 10
                    color: "#00BCD4"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }

                Text {
                    text: "↓ Down: " + systemInfo.netDownSpeed
                    font.family: "DejaVu Sans"
                    font.pixelSize: 10
                    color: "#FF9800"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }
            }

            // Additional stats
            Row {
                spacing: 12

                Text {
                    text: "Packets: " + systemInfo.packetRate
                    font.family: "DejaVu Sans"
                    font.pixelSize: 8
                    color: "#B0B8C8"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }

                Text {
                    text: "Connections: " + systemInfo.activeConnections
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
        currentIndex: 4  // Network tab (index 4)

        // Forward navigation signal to Main.qml
        onNavigationRequested: function(index) {
            root.navigationRequested(index)
        }
    }
}
