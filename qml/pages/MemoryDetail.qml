/*
 * ============================================
 * File: qml/pages/MemoryDetail.qml
 * Description: Memory detail page with large circular gauge
 * ============================================
 */

import QtQuick 2.15
import "../components"

Rectangle {
    id: root

    // ==================== PROPERTIES ====================

    width: 320
    height: 240
    color: "#0F1419"  // Dark background matching Dashboard

    // ==================== SIGNALS FOR NAVIGATION ====================
    signal backRequested()
    signal settingsRequested()
    signal navigationRequested(int index)

    // ==================== MOCK DATA ====================
    // TODO: Replace with systemInfo.xxx when backend ready
    
    property int mockRamUsage: 73            // RAM usage percentage
    property string mockRamUsed: "410MB"     // RAM used
    property string mockRamTotal: "485MB"    // Total RAM
    property string mockRamFree: "75MB"      // Free RAM
    property string mockCache: "80MB"        // Cache size
    
    // ==================== HEADER ====================
    
    DetailHeader {
        id: header
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        title: "MEMORY"

        // Forward navigation signals to Main.qml
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
        
        // ==================== LARGE CIRCULAR GAUGE ====================
        
        Item {
            id: gaugeContainer
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: parent.top
                topMargin: 15     // Space from header
            }
            width: 90
            height: 90

            // Background circle (gray ring)
            Canvas {
                id: backgroundCircle
                anchors.fill: parent
                antialiasing: true

                onPaint: {
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);

                    var centerX = width / 2;
                    var centerY = height / 2;
                    var radius = (width / 2) - 5; // 5px stroke width

                    // Gray background circle
                    ctx.strokeStyle = Qt.rgba(1, 1, 1, 0.1);
                    ctx.lineWidth = 5;
                    ctx.beginPath();
                    ctx.arc(centerX, centerY, radius, 0 , 2 * Math.PI);
                    ctx.stroke();
                }

                Component.onCompleted: requestPaint()
            }

            // Progress arc (blue gradient)
            Canvas {
                id: progressArc
                anchors.fill: parent
                antialiasing: true

                onPaint: {
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);

                    if (root.mockRamUsedM <= 0) return;

                    var centerX = width / 2;
                    var centerY = height / 2;
                    var radius = (width / 2) - 5; // 5px stroke width

                    // Start at top (12 o'clock = -90°)
                    var startAngle = -Math.PI /2;
                    var endAngle = startAngle + (2 * Math.PI * root.mockRamUsage / 100);

                    // Create gradient (Blue -> Cyan)
                    var gradient = ctx.createLinearGradient(
                        centerX - radius, centerY - radius,
                        centerX + radius, centerY + radius
                    );

                    gradient.addColorStop(0, "#2196F3");  // Blue
                    gradient.addColorStop(1, "#00BCD4");  // Cyan
                    
                    ctx.strokeStyle = gradient;
                    ctx.lineWidth = 5;
                    ctx.lineCap = "round";
                    ctx.beginPath();
                    ctx.arc(centerX, centerY, radius, startAngle, endAngle);
                    ctx.stroke();
                }

                Component.onCompleted: requestPaint()
            }

            // Center content (icon + percentage)
            Column {
                anchors.centerIn: parent
                spacing: 2

                // Memory icon
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "💾"
                    font.pixelSize: 20
                    font.family: "DejaVu Sans"
                    renderType: Text.NativeRendering 
                }

                // Percentage Text
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: root.mockRamUsage + "%"
                    font.family: "DejaVu Sans"
                    font.pixelSize: 24
                    font.bold: true
                    color: "#FFFFFF"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }
            }    
        }

        // ==================== "RAM USAGE" LABEL ====================

        Text {
            id: usageLabel
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: gaugeContainer.bottom
                topMargin: 8
            }
            text: "RAM Usage"
            font.family: "DejaVu Sans"
            font.pixelSize: 10
            font.bold: true
            color: "#B0B8C8"  // Secondary text color
            renderType: Text.NativeRendering
            antialiasing: false
            font.hintingPreference: Font.PreferFullHinting
        }

        // ==================== STATS CARD ====================
        Rectangle {
            id: starsCard
            anchors {
                horizontalCenter: parent.horizontalCenter
                top: usageLabel.bottom
                topMargin: 10
            }
            width: 304
            height: 30
            color: "#1E2539"  // Card background
            radius: 8
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.1)

            Column {
                anchors {
                    left: parent.left
                    leftMargin: 16
                    verticalCenter: parent.verticalCenter 
                }
                spacing: 2
                
                // Used memory text
                Text {
                    text: "Used: " + root.mockRamUsed + " / " + root.mockRamTotal
                    font.family: "DejaVu Sans"
                    font.pixelSize: 10
                    color: "#FFFFFF"
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting
                }

                // Free and cache text
                Row {
                    spacing: 10

                    Text {
                        text: "Free: " + root.mockRamFree
                        font.family: "DejaVu Sans"
                        font.pixelSize: 8
                        color: "#B0B8C8"
                        renderType: Text.NativeRendering
                        antialiasing: false
                        font.hintingPreference: Font.PreferFullHinting
                    }

                    Text {
                        text: "Cache: " + root.mockCache
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
    }

    // ==================== BOTTOM NAVIGATION ====================

    BottomNav {
        id: bottomNav
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        currentIndex: 2  // Memory tab (index 2)

        // Forward navigation signal to Main.qml
        onNavigationRequested: function(index) {
            root.navigationRequested(index)
        }
    }
   

}
