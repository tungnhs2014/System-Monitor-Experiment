/*
 * ============================================
 * File: qml/views/pages/Dashboard.qml
 * ============================================
 */

import QtQuick 2.15
import "../components"

Rectangle {
    id: root

    width: 320
    height: 240
    color: "#0A0E14"  

    // ==================== SIGNALS FOR NAVIGATION ====================
    signal navigationRequested(int index)
    signal settingsRequested()

    // Top Bar
    TopBar {
        id: topBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        currentTime: Qt.formatTime(new Date(), "hh:mm")
        hostname: systemInfo.hostname
        
        onSettingsClicked: root.settingsRequested()
    }

    // Content Area
    Item {
        id: contentArea
        anchors.top: topBar.bottom
        anchors.bottom: bottomNav.top
        anchors.left: parent.left
        anchors.right: parent.right

        // ==================== CPU CARD ====================
        CardContainer {
            x: 8
            y: 8

            Column {
                anchors.fill: parent
                spacing: 0

                Row {
                    width: parent.width
                    height: 12
                    spacing: 2

                    Image {
                        source: "qrc:/assets/icons/cpu.png"
                        width: 10
                        height: 10
                        sourceSize: Qt.size(10, 10)
                        smooth: false
                    }
                    Text {
                        text: "CPU"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 10
                        font.bold: true
                        font.hintingPreference: Font.PreferFullHinting
                        color: "#FFFFFF"
                        renderType: Text.NativeRendering
                        antialiasing: false 
                    }
                }

                Item {
                    width: parent.width
                    height: 45

                    CircularGauge {
                        width: 45
                        height: 45
                        value: systemInfo.cpuUsage
                        startColor: "#9C27B0"
                        endColor: "#2196F3"
                        fontSize: 14
                        strokeWidth: 2
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.horizontalCenterOffset: -14

                        MouseArea {
                            anchors.fill: parent
                            anchors.margins: -10 
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                root.navigationRequested(1)  // Navigate to CPU (index 1)
                            }
                        }
                    }

                    Column {
                        width: 35
                        spacing: 2
                        anchors.right: parent.right

                        Text {
                            text: "Temp"
                            font.pixelSize: 8
                            font.bold: true
                            font.family: "DejaVu Sans"
                            font.hintingPreference: Font.PreferFullHinting
                            color: "#B0B8C8"
                            renderType: Text.NativeRendering
                            antialiasing: false
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Text {
                            text: systemInfo.cpuTemp + "°"
                            font.pixelSize: 10
                            font.bold: true
                            font.family: "DejaVu Sans"
                            font.hintingPreference: Font.PreferFullHinting
                            color: systemInfo.cpuTemp > 80 ? "#FF9800" : "#4CAF50"
                            renderType: Text.NativeRendering
                            antialiasing: false
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }

                Item {
                    width: parent.width
                    height: 10

                    Text {
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "CLK " + systemInfo.cpuClock
                        font.pixelSize: 8
                        font.bold: true
                        font.family: "DejaVu Sans"
                        font.hintingPreference: Font.PreferFullHinting
                        color: "#B0B8C8"
                        renderType: Text.NativeRendering
                        antialiasing: false
                    }
                }
            }
        }

        // ==================== GPU CARD ====================
        CardContainer {
            x: 116
            y: 8

            Column {
                anchors.fill: parent
                spacing: 0

                Row {
                    width: parent.width
                    height: 12
                    spacing: 2

                    Image {
                        source: "qrc:/assets/icons/gpu.png"
                        width: 10
                        height: 10
                        sourceSize: Qt.size(10, 10)
                        smooth: false
                    }
                    Text {
                        text: "GPU"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 10
                        font.bold: true
                        font.hintingPreference: Font.PreferFullHinting
                        color: "#FFFFFF"
                        renderType: Text.NativeRendering
                        antialiasing: false
                    }
                }

                Item {
                    width: parent.width
                    height: 45

                    CircularGauge {
                        width: 45
                        height: 45
                        value: systemInfo.gpuUsage
                        startColor: "#00BCD4"
                        endColor: "#2196F3"
                        fontSize: 14
                        strokeWidth: 2
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.horizontalCenterOffset: -14

                        MouseArea {
                            anchors.fill: parent
                            anchors.margins: -10 
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                root.navigationRequested(2)  // Navigate to Memory (index 2)
                            }
                        }
                    }

                    Column {
                        width: 32
                        spacing: 0
                        anchors.right: parent.right
                        anchors.rightMargin: 2

                        Text {
                            text: "Temp"
                            font.pixelSize: 8
                            font.bold: true
                            font.family: "DejaVu Sans"
                            font.hintingPreference: Font.PreferFullHinting
                            color: "#B0B8C8"
                            renderType: Text.NativeRendering
                            antialiasing: false
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Text {
                            text: systemInfo.gpuTemp + "°"
                            font.pixelSize: 11
                            font.bold: true
                            font.family: "DejaVu Sans"
                            font.hintingPreference: Font.PreferFullHinting
                            color: systemInfo.gpuTemp > 60 ? "#FF9800" : "#4CAF50"
                            renderType: Text.NativeRendering
                            antialiasing: false
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }

                Item {
                    width: parent.width
                    height: 10

                    Text {
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "MEM " + systemInfo.gpuMemUsage + "M"
                        font.pixelSize: 8
                        font.bold: true
                        font.family: "DejaVu Sans"
                        font.hintingPreference: Font.PreferFullHinting
                        color: "#B0B8C8"
                        renderType: Text.NativeRendering
                        antialiasing: false
                    }
                }
            }
        }

        // ==================== DATE CARD ====================
        CardContainer {
            x: 224
            y: 8

            Column {
                anchors.fill: parent
                spacing: 0

                Row {
                    width: parent.width
                    height: 12
                    spacing: 2

                    Image {
                        source: "qrc:/assets/icons/calendar.png"
                        width: 10
                        height: 10
                        sourceSize: Qt.size(10, 10)
                        smooth: false
                    }
                    Text {
                        text: "DATE"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 10
                        font.bold: true
                        font.hintingPreference: Font.PreferFullHinting
                        color: "#FFFFFF"
                        renderType: Text.NativeRendering
                        antialiasing: false
                    }
                }

                Item {
                    width: parent.width
                    height: 49

                    Text {
                        anchors.centerIn: parent
                        text: Qt.formatDate(new Date(), "MM/dd")
                        font.family: "DejaVu Sans"
                        font.pixelSize: 12
                        font.bold: true
                        font.hintingPreference: Font.PreferFullHinting
                        color: "#FFFFFF"
                        renderType: Text.NativeRendering
                        antialiasing: false
                    }
                }
            }
        }

        // ==================== RAM CARD ====================
        CardContainer {
            x: 8
            y: 91

            Column {
                anchors.fill: parent
                spacing: 0

                Row {
                    width: parent.width
                    height: 12
                    spacing: 2

                    Image {
                        source: "qrc:/assets/icons/memory.png"
                        width: 10
                        height: 10
                        sourceSize: Qt.size(10, 10)
                        smooth: false
                    }
                    Text {
                        text: "RAM"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 10
                        font.bold: true
                        font.hintingPreference: Font.PreferFullHinting
                        color: "#FFFFFF"
                        renderType: Text.NativeRendering
                        antialiasing: false
                    }
                }

                Item {
                    width: parent.width
                    height: 45

                    CircularGauge {
                        width: 45
                        height: 45
                        value: systemInfo.ramUsage
                        startColor: "#2196F3"
                        endColor: "#2196F3"
                        useGradient: false
                        fontSize: 14
                        strokeWidth: 2
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.horizontalCenterOffset: -14

                        MouseArea {
                            anchors.fill: parent
                            anchors.margins: -10 
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                root.navigationRequested(3)  // Navigate to RAM (index 3)
                            }
                        }
                    }

                    Column {
                        width: 32
                        spacing: 0
                        anchors.right: parent.right
                        anchors.rightMargin: 2

                        Text {
                            text: "USED"
                            font.pixelSize: 8
                            font.bold: true
                            font.family: "DejaVu Sans"
                            font.hintingPreference: Font.PreferFullHinting
                            color: "#B0B8C8"
                            renderType: Text.NativeRendering
                            antialiasing: false
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Text {
                            text: systemInfo.ramUsed
                            font.pixelSize: 9
                            font.bold: true
                            font.family: "DejaVu Sans"
                            font.hintingPreference: Font.PreferFullHinting
                            color: "#FFFFFF"
                            renderType: Text.NativeRendering
                            antialiasing: false
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }

                Item {
                    width: parent.width
                    height: 10

                    Text {
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "FREE " + systemInfo.ramFree
                        font.pixelSize: 8
                        font.bold: true
                        font.family: "DejaVu Sans"
                        font.hintingPreference: Font.PreferFullHinting
                        color: "#B0B8C8"
                        renderType: Text.NativeRendering
                        antialiasing: false
                    }
                }
            }
        }

        // ==================== HDD CARD ====================
        CardContainer {
            x: 116
            y: 91

            Column {
                anchors.fill: parent
                spacing: 0

                Row {
                    width: parent.width
                    height: 12
                    spacing: 2

                    Image {
                        source: "qrc:/assets/icons/hard-drive.png"
                        width: 10
                        height: 10
                        sourceSize: Qt.size(10, 10)
                        smooth: false
                    }
                    Text {
                        text: "HDD"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 10
                        font.bold: true
                        font.hintingPreference: Font.PreferFullHinting
                        color: "#FFFFFF"
                        renderType: Text.NativeRendering
                        antialiasing: false
                    }
                }

                Item {
                    width: parent.width
                    height: 45

                    Column {
                        anchors.centerIn: parent
                        spacing: 2

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: systemInfo.hddUsage + "%"
                            font.pixelSize: 14
                            font.bold: true
                            font.family: "DejaVu Sans"
                            font.hintingPreference: Font.PreferFullHinting
                            color: "#FFFFFF"
                            renderType: Text.NativeRendering
                            antialiasing: false

                            MouseArea {
                                anchors.fill: parent
                                anchors.margins: -10 
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    root.navigationRequested(4)  // Navigate to Network (index 3)
                            }
                        }
                        }

                        ProgressBar {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: 78
                            height: 4
                            value: systemInfo.hddUsage
                            fillColor: "#FF9800"
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: systemInfo.hddTemp + "°C"
                            font.pixelSize: 8
                            font.bold: true
                            font.family: "DejaVu Sans"
                            font.hintingPreference: Font.PreferFullHinting
                            color: systemInfo.hddTemp > 50 ? "#FF9800" : "#4CAF50"
                            renderType: Text.NativeRendering
                            antialiasing: false
                        }
                    }
                }

                Item {
                    width: parent.width
                    height: 6

                    Row {
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 3

                        Text {
                            text: "NET"
                            font.pixelSize: 7
                            font.bold: true
                            font.family: "DejaVu Sans"
                            font.hintingPreference: Font.PreferFullHinting
                            color: "#B0B8C8"
                            renderType: Text.NativeRendering
                            antialiasing: false
                        }
                        Text {
                            text: "↓" + systemInfo.net
                            font.pixelSize: 7
                            font.bold: true
                            font.family: "DejaVu Sans"
                            font.hintingPreference: Font.PreferFullHinting
                            color: "#4CAF50"
                            renderType: Text.NativeRendering
                            antialiasing: false
                        }
                        Text {
                            text: "↑" + systemInfo.netUp
                            font.pixelSize: 7
                            font.bold: true
                            font.family: "DejaVu Sans"
                            font.hintingPreference: Font.PreferFullHinting
                            color: "#F44336"
                            renderType: Text.NativeRendering
                            antialiasing: false
                        }
                    }
                }
            }
        }

        // ==================== TIME CARD ====================
        CardContainer {
            x: 224
            y: 91

            Column {
                anchors.fill: parent
                spacing: 0

                Row {
                    width: parent.width
                    height: 12
                    spacing: 2

                    Image {
                        source: "qrc:/assets/icons/clock.png"
                        width: 10
                        height: 10
                        sourceSize: Qt.size(10, 10)
                        smooth: false
                    }
                    Text {
                        text: "TIME"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 10
                        font.bold: true
                        font.hintingPreference: Font.PreferFullHinting
                        color: "#FFFFFF"
                        renderType: Text.NativeRendering
                        antialiasing: false
                    }
                }

                Item {
                    width: parent.width
                    height: 49

                    Text {
                        anchors.centerIn: parent
                        text: Qt.formatTime(new Date(), "hh:mm") 
                        font.family: "DejaVu Sans"
                        font.pixelSize: 18
                        font.bold: true
                        font.hintingPreference: Font.PreferFullHinting
                        color: "#FFFFFF"
                        renderType: Text.NativeRendering
                        antialiasing: false
                    }
                }
            }
            
            Timer {
                interval: 1000
                running: true
                repeat: true
                onTriggered: parent.children[0].children[1].children[0].text = Qt.formatTime(new Date(), "hh:mm")
            }
        }
    }

    // Bottom Navigation
    BottomNav {
        id: bottomNav
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        currentIndex: 0

        // Forward navigation signal to Main.qml
        onNavigationRequested: function(index) {
            root.navigationRequested(index)
        }
    }
}
