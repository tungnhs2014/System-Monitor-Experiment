/*
 * ============================================
 * File: qml/views/pages/Settings.qml
 * Description: Settings page
 * ============================================
 */

import QtQuick 2.15
import "../components"
import ".." as Root

Rectangle {
    id: root

    // ==================== PROPERTIES ====================

    width: 320
    height: 240
    color: "#0F1419"

    // ==================== SIGNALS FOR NAVIGATION ====================
    signal backRequested()
    signal settingsRequested()

    property string mockHostname: "raspberrypi"
    property string mockOsVersion: "Raspberry Pi OS"
    property string mockKernel: "Linux 5.10.103"
    property string mockUptime: "3d 4h 15m 30s"
    property string mockSystemTime: "03:00:48 [NTP]"

    property int mockUpdateInterval: 2
    property bool mockSoundAlert: false

    property int mockCpuWarn: 70
    property int mockCpuCrit: 90
    property int mockRamWarn: 75

    property var mockLogs: [
        {time: "03:00:45", level: "INFO", message: "System monitor started"},
        {time: "03:00:46", level: "INFO", message: "Network connected: eth0"},
        {time: "03:00:48", level: "WARN", message: "High CPU temperature: 85°C"},
        {time: "03:00:50", level: "ERROR", message: "Storage 95% full - cleanup needed"},
        {time: "03:00:52", level: "INFO", message: "Settings loaded successfully"}
    ]

    // ==================== HEADER ====================
    DetailHeader {
        id: header
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        title: "SETTINGS"
        onBackClicked: root.backRequested()
        onSettingsClicked: {}
    }

    // ==================== TAB BAR ====================

    TabBar {
        id: tabBar
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
        }
        tabs: ["System", "Monitor", "Warning", "Logs"]
        currentIndex: 0

        onTabClicked: function(index) {
            if (QML_DEBUG_ENABLED) console.log("Tab switched to:", tabs[index])
        }
    }

    // ==================== CONTENT AREA ====================

    Item {
        id: contentArea
        anchors {
            top: tabBar.bottom
            left: parent.left
            right: parent.right
            bottom: tabBar.currentIndex === 0 ? bottomNav.top : saveButton.top
            margins: 8
        }

        // ==================== TAB 1: SYSTEM INFO ====================
        
        Flickable {
            anchors.fill: parent
            contentHeight: systemColumn.height
            clip: true
            visible: tabBar.currentIndex === 0
            
            Column {
                id: systemColumn
                width: parent.parent.width
                spacing: 10

                InfoRow { label: "Hostname:"; value: systemInfo.hostname }
                InfoRow { label: "OS Version:"; value: systemInfo.osVersion }
                InfoRow { label: "Kernel:"; value: systemInfo.kernelVersion }
                InfoRow { label: "Uptime:"; value: systemInfo.uptime }
                InfoRow { label: "System Time:"; value: systemInfo.systemTime }

                Item { height: 10 }

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 140
                    height: 28
                    text: "Reboot"
                    buttonColor: "#FF9800"
                    onClicked: rebootDialog.show()
                }

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 140
                    height: 28
                    text: "Shutdown"
                    buttonColor: "#F44336"
                    onClicked: shutdownDialog.show()
                }
            }
        }

        // ==================== TAB 2: MONITOR SETTINGS ====================

        Column {
            anchors.fill: parent
            spacing: 12
            visible: tabBar.currentIndex === 1
            
            // Update interval
            Row {
                width: parent.width
                spacing: 10

                Text {
                    text: "Update Interval:"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 9
                    color: "#FFFFFF"
                    anchors.verticalCenter: parent.verticalCenter
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }

                // Dropdown (simplified as button for now)
                Rectangle {
                    width: 60
                    height: 24
                    radius: 4
                    color: "#1E2539"
                    border.width: 1
                    border.color: Qt.rgba(1, 1, 1, 0.2)

                    Row {
                        anchors.centerIn: parent
                        spacing: 4

                        Text {
                            text: systemInfo.updateInterval + "s"
                            font.pixelSize: 9
                            color: "#FFFFFF"
                            renderType: Text.NativeRendering
                            antialiasing: false
                            font.hintingPreference: Font.PreferFullHinting
                        }

                        Text {
                            text: "▼"
                            font.family: "DejaVu Sans"
                            font.pixelSize: 7
                            color: "#B0B8C8"
                            renderType: Text.NativeRendering
                            antialiasing: false
                            font.hintingPreference: Font.PreferFullHinting
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: if (QML_DEBUG_ENABLED) console.log("Update interval dropdown clicked")
                    }
                }
            }

            // Removed Dark Mode toggle
        }

        // ==================== TAB 3: WARNING THRESHOLDS ====================

        Flickable {
            anchors.fill: parent
            contentHeight: warningColumn.height
            clip: true
            visible: tabBar.currentIndex === 2
            
            Column {
                id: warningColumn
                width: parent.parent.width
                spacing: 12

                Text {
                    text: "CPU Thresholds"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 10
                    font.bold: true
                    color: "#FFFFFF"
                    renderType: Text.NativeRendering
                }

                Row {
                    width: parent.width
                    spacing: 10

                    Text {
                        text: "Warning:"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 9
                        color: "#FFFFFF"
                        anchors.verticalCenter: parent.verticalCenter
                        width: 70
                        renderType: Text.NativeRendering
                    }

                    SpinBox {
                        value: systemInfo.cpuWarnThreshold
                        minValue: 50
                        maxValue: 95
                        step: 5
                        suffix: "%"
                        anchors.verticalCenter: parent.verticalCenter
                        onValueChanged: systemInfo.cpuWarnThreshold = value
                    }
                }

                // CPU critical
                Row {
                    width: parent.width
                    spacing: 10

                    Text {
                        text: "Critical:"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 9
                        color: "#FFFFFF"
                        anchors.verticalCenter: parent.verticalCenter
                        width: 70
                        renderType: Text.NativeRendering
                    }

                    SpinBox {
                        value: systemInfo.cpuCritThreshold
                        minValue: 60
                        maxValue: 100
                        step: 5
                        suffix: "%"
                        anchors.verticalCenter: parent.verticalCenter
                        onValueChanged: systemInfo.cpuCritThreshold = value
                    }
                }

                Item { height: 8 }

                Text {
                    text: "RAM Thresholds"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 10
                    font.bold: true
                    color: "#FFFFFF"
                    renderType: Text.NativeRendering
                }

                Row {
                    width: parent.width
                    spacing: 10

                    Text {
                        text: "Warning:"
                        font.family: "DejaVu Sans"
                        font.pixelSize: 9
                        color: "#FFFFFF"
                        anchors.verticalCenter: parent.verticalCenter
                        width: 70
                        renderType: Text.NativeRendering
                    }

                    SpinBox {
                        value: systemInfo.ramWarnThreshold
                        minValue: 50
                        maxValue: 95
                        step: 5
                        suffix: "%"
                        anchors.verticalCenter: parent.verticalCenter
                        onValueChanged: systemInfo.ramWarnThreshold = value
                    }
                }
            }
        }

        // ==================== TAB 4: SYSTEM LOGS ====================

        Column {
            anchors.fill: parent
            spacing: 4
            visible: tabBar.currentIndex === 3

            Text {
                text: "Recent System Logs"
                font.family: "DejaVu Sans"
                font.pixelSize: 10
                font.bold: true
                color: "#FFFFFF"
                renderType: Text.NativeRendering
            }

            ListView {
                width: parent.width
                height: parent.height - 20
                clip: true
                model: systemInfo.systemLogs

                delegate: Rectangle {
                    width: ListView.view.width
                    height: 18
                    color: index % 2 === 0 ? "#1E2539" : "#0F1419"

                    Row {
                        anchors.fill: parent
                        anchors.leftMargin: 4
                        spacing: 6

                        Text {
                            text: modelData.time
                            font.family: "DejaVu Sans Mono"
                            font.pixelSize: 7
                            color: "#B0B8C8"
                            anchors.verticalCenter: parent.verticalCenter
                            renderType: Text.NativeRendering
                        }

                        Rectangle {
                            width: 32
                            height: 12
                            radius: 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: {
                                switch(modelData.level) {
                                    case "ERROR": return "#F44336"
                                    case "WARN": return "#FF9800"
                                    case "INFO": return "#4CAF50"
                                    default: return "#2196F3"
                                }
                            }

                            Text {
                                anchors.centerIn: parent
                                text: modelData.level
                                font.family: "DejaVu Sans"
                                font.pixelSize: 6
                                font.bold: true
                                color: "#FFFFFF"
                                renderType: Text.NativeRendering
                            }
                        }

                        Text {
                            text: modelData.message
                            font.family: "DejaVu Sans"
                            font.pixelSize: 7
                            color: "#FFFFFF"
                            anchors.verticalCenter: parent.verticalCenter
                            renderType: Text.NativeRendering
                            elide: Text.ElideRight
                            width: 180
                        }
                    }
                }
            }
        }
    }

    // ==================== SAVE BUTTON ====================

    Button {
        id: saveButton
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: bottomNav.top
            bottomMargin: 8
        }
        width: 304
        height: 24
        text: "Save Changes"
        buttonColor: "#4CAF50"  // Green
        visible: tabBar.currentIndex !== 0  // Hide on System tab (index 0)

        onClicked: {
            if (QML_DEBUG_ENABLED) console.log("Saving settings...")
            if (QML_DEBUG_ENABLED) console.log("Update interval:", systemInfo.updateInterval)
            if (QML_DEBUG_ENABLED) console.log("Sound alert:", systemInfo.soundAlert)
            if (QML_DEBUG_ENABLED) console.log("CPU warn:", systemInfo.cpuWarnThreshold)
            if (QML_DEBUG_ENABLED) console.log("CPU crit:", systemInfo.cpuCritThreshold)
            if (QML_DEBUG_ENABLED) console.log("RAM warn:", systemInfo.ramWarnThreshold)

            // Save to backend
            systemInfo.saveSettings()

            // Show success feedback (could add toast/snackbar)
            if (QML_DEBUG_ENABLED) console.log("Settings saved successfully!")
        }
    }

    // ==================== CONFIRMATION DIALOGS ====================

    ConfirmDialog {
        id: rebootDialog
        title: "Reboot System"
        message: "Are you sure you want to reboot?\nAll unsaved data will be lost."
        confirmText: "Reboot"
        confirmColor: "#FF9800"
        onAccepted: systemInfo.reboot()
    }

    ConfirmDialog {
        id: shutdownDialog
        title: "Shutdown System"
        message: "Are you sure you want to shutdown?\nThe system will power off."
        confirmText: "Shutdown"
        confirmColor: "#F44336"
        onAccepted: systemInfo.shutdown()
    }

    // ==================== BOTTOM NAVIGATION ====================

    BottomNav {
        id: bottomNav
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        currentIndex: 0
    }

    // ==================== HELPER COMPONENTS ====================

    component InfoRow: Row {
        property string label: ""
        property string value: ""
        spacing: 8

        Text {
            text: label
            font.family: "DejaVu Sans"
            font.pixelSize: 9
            color: "#B0B8C8"
            width: 100
            renderType: Text.NativeRendering
        }

        Text {
            text: value
            font.family: "DejaVu Sans"
            font.pixelSize: 9
            font.bold: true
            color: "#FFFFFF"
            renderType: Text.NativeRendering
        }
    }
}
