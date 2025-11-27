/*
 * ============================================
 * File: qml/pages/Settings.qml
 * Description: Settings page with 4 tabs
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

    // ==================== MOCK DATA ====================
    
    // System tab
    property string mockHostname: "raspberrypi"
    property string mockOsVersion: "Raspberry Pi OS"
    property string mockKernel: "Linux 5.10.103"
    property string mockUptime: "3d 4h 15m 30s"
    property string mockSystemTime: "03:00:48 [NTP]"

    // Monitor tab
    property int mockUpdateInterval: 2
    property bool mockDarkMode: true
    property bool mockSoundAlert: false

    // Warning tab
    property int mockCpuWarn: 70
    property int mockCpuCrit: 90
    property int mockRamWarn: 75

    // Logs tab 
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

        onBackClicked: {
            root.backRequested()
        }

        onSettingsClicked: {
            // Already in Settings, do nothing
        }
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
            console.log("Tab switched to:", tabs[index])
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
        
        Column {
            anchors.fill: parent
            spacing: 8
            visible: tabBar.currentIndex === 0

            // System info card
            Rectangle {
                width: parent.width
                height: 100
                color: "#1E2539"
                radius: 8
                border.width: 1
                border.color: Qt.rgba(1, 1, 1, 0.1)

                Column {
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                        margins: 10
                    }
                    spacing: 6
                    // Info rows
                    Repeater {
                        model: [
                            {label: "Hostname:", value: systemInfo.hostname},
                            {label: "OS Version:", value: systemInfo.osVersion},
                            {label: "Kernel:", value: systemInfo.kernelVersion},
                            {label: "Uptime:", value: systemInfo.uptime},
                            {label: "System Time:", value: systemInfo.systemTime}
                        ]

                        Row {
                            spacing: 8

                            Text {
                                text: modelData.label
                                font.family: "DejaVu Sans"
                                font.pixelSize: 8
                                font.bold: true
                                color: "#B0B8C8"
                                width: 80
                                renderType: Text.NativeRendering
                                antialiasing: false
                                font.hintingPreference: Font.PreferFullHinting
                            }
                            
                            Text {
                                text: modelData.value
                                font.family: "DejaVu Sans"
                                font.pixelSize: 8
                                font.bold: true
                                color: "#FFFFFF"
                                renderType: Text.NativeRendering
                                antialiasing: false
                                font.hintingPreference: Font.PreferFullHinting
                            }
                        }
                    }
                }
            }

            // System control buttons
            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10

                Button {
                    text: "Reboot"
                    buttonColor: "#FF9800"  // Orange
                    width: 90
                    height: 26
                    onClicked: {
                        console.log("Reboot requested")
                        // TODO: Show confirmation dialog
                        systemInfo.reboot()
                    }
                }

                Button {
                    text: "Shutdown"
                    buttonColor: "#F44336"  // Red
                    width: 90
                    height: 26
                    onClicked: {
                        console.log("Shutdown requested")
                        // TODO: Show confirmation dialog
                        systemInfo.shutdown()
                    }
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
                        onClicked: {
                            console.log("Update interval dropdown clicked")
                            // TODO: Show dropdown menu
                        }
                    }
                }
            }

            // Dark mode toggle
            Row {
                width: parent.width
                spacing: 10

                Text {
                    text: "Dark Mode:"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 9
                    color: "#FFFFFF"
                    anchors.verticalCenter: parent.verticalCenter
                    width: 100
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }

                ToggleSwitch {
                    checked: systemInfo.darkMode
                    onToggled: function(state) {
                        systemInfo.darkMode = state
                        console.log("Dark mode:", state ? "ON" : "OFF")
                    }
                }
            }
        }
        
        // ==================== TAB 3: WARNING THRESHOLDS ====================
        
        Column {
            anchors.fill: parent
            spacing: 12
            visible: tabBar.currentIndex === 2

            // CPU warning
            Row {
                width: parent.width
                spacing: 10

                Text {
                    text: "CPU Warning:"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 9
                    color: "#FFFFFF"
                    anchors.verticalCenter: parent.verticalCenter
                    width: 100
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }

                InputField {
                    id: cpuWarnInput
                    value: systemInfo.cpuWarnThreshold
                    minValue: 50
                    maxValue: 95
                    suffix: "%"
                    onValueChanged: {
                        systemInfo.cpuWarnThreshold = cpuWarnInput.value
                        console.log("CPU warning threshold:", cpuWarnInput.value)
                    }
                }
            }

            // CPU critical
            Row {
                width: parent.width
                spacing: 10
                
                Text {
                    text: "CPU Critical:"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 9
                    color: "#FFFFFF"
                    anchors.verticalCenter: parent.verticalCenter
                    width: 100
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }
                
                InputField {
                    id: cpuCritInput
                    value: systemInfo.cpuCritThreshold
                    minValue: 50
                    maxValue: 95
                    suffix: "%"
                    onValueChanged: {
                        systemInfo.cpuCritThreshold = cpuCritInput.value
                        console.log("CPU critical threshold:", cpuCritInput.value)
                    }
                }
            }

            // RAM warning
            Row {
                width: parent.width
                spacing: 10
                
                Text {
                    text: "RAM Warning:"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 9
                    color: "#FFFFFF"
                    anchors.verticalCenter: parent.verticalCenter
                    width: 100
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }
                
                InputField {
                    id: ramWarnInput
                    value: systemInfo.ramWarnThreshold
                    minValue: 50
                    maxValue: 95
                    suffix: "%"
                    onValueChanged: {
                        systemInfo.ramWarnThreshold = ramWarnInput.value
                        console.log("RAM warning threshold:", ramWarnInput.value)
                    }
                }
            }
        }

        // ==================== TAB 4: SYSTEM LOGS ====================

        Rectangle {
            anchors.fill: parent
            visible: tabBar.currentIndex === 3
            color: "#1E2539"
            radius: 8
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.1)

            ListView {
                id: logListView
                anchors {
                    fill: parent
                    margins: 6
                }
                clip: true
                spacing: 4
                model: systemInfo.systemLogs

                delegate: Row {
                    spacing: 6

                    // Time
                    Text {
                        text: modelData.time
                        font.family: "DejaVu Sans"
                        font.pixelSize: 7
                        color: "#B0B8C8"
                        width: 50
                        renderType: Text.NativeRendering
                        antialiasing: false
                        font.hintingPreference: Font.PreferFullHinting
                    }

                    // Level
                    Rectangle {
                        width: 36
                        height: 12
                        radius: 3
                        color: {
                            if (modelData.level == "ERROR") return "#F44336"
                            if (modelData.level === "WARN") return "#FF9800"
                            return "#2196F3" // INFO
                        }

                        Text {
                            anchors.centerIn: parent
                            text: modelData.level
                            font.family: "DejaVu Sans"
                            font.pixelSize: 6
                            font.bold: true
                            color: "#FFFFFF"
                            renderType: Text.NativeRendering
                            antialiasing: false
                            font.hintingPreference: Font.PreferFullHinting
                        }
                    }

                    // Message
                    Text {
                        text: modelData.message
                        font.family: "DejaVu Sans"
                        font.pixelSize: 7
                        color: "#FFFFFF"
                        width: 200
                        wrapMode: Text.WordWrap
                        renderType: Text.NativeRendering
                        antialiasing: false
                        font.hintingPreference: Font.PreferFullHinting
                    }
                }

                // Scrollbar indicator
                Rectangle {
                    anchors {
                        right: parent.right
                        top: parent.top
                        bottom: parent.bottom
                    }
                    width: 3
                    color: Qt.rgba(1, 1, 1, 0.1)
                    radius: 1.5

                    Rectangle {
                        width: parent.width
                        height: {
                            var ratio = logListView.height / logListView.contentHeight
                            return Math.max(parent.height * ratio, 20)
                        }
                        y: {
                            var ratio = logListView.contentY / (logListView.contentHeight - logListView.height)
                            return ratio * (parent.height - height)
                        }
                        color: "#2196F3"
                        radius: 1.5
                        visible: logListView.contentHeight > logListView.height
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
        height: 30
        text: "Save Changes"
        buttonColor: "#4CAF50"  // Green
        visible: tabBar.currentIndex !== 0  // Hide on System tab (index 0)

        onClicked: {
            console.log("Saving settings...")
            console.log("Update interval:", systemInfo.updateInterval)
            console.log("Dark mode:", systemInfo.darkMode)
            console.log("Sound alert:", systemInfo.soundAlert)
            console.log("CPU warn:", systemInfo.cpuWarnThreshold)
            console.log("CPU crit:", systemInfo.cpuCritThreshold)
            console.log("RAM warn:", systemInfo.ramWarnThreshold)

            // Save to backend
            systemInfo.saveSettings()

            // Show success feedback (could add toast/snackbar)
            console.log("Settings saved successfully!")
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
