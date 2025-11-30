/*
 * ============================================
 * File: qml/views/components/PartitionBar.qml
 * Description: Storage partition usage bar with details
 * ============================================
 */

import QtQuick 2.15

Item {
    id: root

    // ==================== PROPERTIES ====================

    width: 304
    height: 50  // Compact layout for 2 partitions

    property string partitionName: "Partition"
    property string totalSize: "0 GB"
    property int usage: 0               // Usage percentage (0-100)
    property string usedSize: "0 GB"
    property string freeSize: "0 GB"

    // Auto-calculated properties
    property color barColor: _calculateColor()

    // ==================== PARTITION LABEL ====================

    Text {
        id: labelText
        anchors {
            left: parent.left
            top: parent.top
        }
        text: root.partitionName + " (" + root.totalSize + ")"
        font.family: "DejaVu Sans"
        font.pixelSize: 10
        font.bold: true
        color: "#FFFFFF"

        // RGB565 optimizations
        renderType: Text.NativeRendering
        antialiasing: false
        font.hintingPreference: Font.PreferFullHinting
    }

    // ==================== PROGRESS BAR ROW ====================

    Row {
        id: barRow
        anchors {
            left: parent.left
            top: labelText.bottom
            topMargin: 4
        }
        spacing: 8

        // ==================== BAR CONTAINER ====================

        Item {
            width: 240
            height: 8

            // Background bar
            Rectangle {
                id: backgroundBar
                anchors.fill: parent
                radius: 4
                color: Qt.rgba(1, 1, 1, 0.1)
            }

            // Fill bar
            Rectangle {
                id: fillBar
                anchors {
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }
                width: (parent.width * root.usage) / 100
                height: parent.height
                radius: parent.height / 2
                color: root.barColor

                // Smooth width animation
                Behavior on width {
                    NumberAnimation {
                        duration: 300
                        easing.type: Easing.OutCubic
                    }
                }

                // Smooth color transition
                Behavior on color {
                    ColorAnimation {
                        duration: 200
                    }
                }
            }
        }

        // ==================== PERCENTAGE LABEL ====================
        
        Text {
            id: percentageText
            text: root.usage + "%"
            font.family: "DejaVu Sans"
            font.pixelSize: 10
            font.bold: true
            color: root.barColor
            anchors.verticalCenter: parent.verticalCenter

            // RGB565 optimizations
            renderType: Text.NativeRendering
            antialiasing: false
            font.hintingPreference: Font.PreferFullHinting

            // Smooth color transition
            Behavior on color {
                ColorAnimation {
                    duration: 200
                }
            }
        }
    }

    // ==================== USED/FREE DETAILS ====================

    Text {
        id: detailsText
        anchors {
            left: parent.left
            top: barRow.bottom
            topMargin: 4
        }
        text: "Used: " + root.usedSize + " | Free: " + root.freeSize
        font.family: "DejaVu Sans"
        font.pixelSize: 8
        color: "#B0B8C8"

        // RGB565 optimizations
        renderType: Text.NativeRendering
        antialiasing: false
        font.hintingPreference: Font.PreferFullHinting
    }

    // ==================== COLOR LOGIC ====================
    function _calculateColor() {
        if (root.usage < 75) {
            return "#4CAF50";   // Green - healthy, plenty of space
        }
        else if (root.usage < 90) {
            return "#FF9800";   // Orange - warning, running low
        }
        else {
            return "#F44336";   // Red - critical, almost full
        }
    }

    // Recalculate color when usage changes
    onUsageChanged: {
        barColor = _calculateColor();
    }
}