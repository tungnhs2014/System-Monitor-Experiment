/*
 * ============================================
 * File: qml/components/TabBar.qml
 * Description: Horizontal tab bar with 4 tabs
 * ============================================
 */

import QtQuick 2.15

Rectangle {
    id: root
    
    // ==================== PROPERTIES ====================
    
    width: 320
    height: 20
    color: "#1E2539"  

    property var tabs: ["Tab 1", "Tab 2", "Tab 3", "Tab 4"]
    property int currentIndex: 0

    // Colors
    property color activeColor:  "#2196F3"      // Blue
    property color inactiveColor: "#B0B8C8"    // Gray
    property color backgroundColor: "#1E2539"
    property color indicatorColor: "#2196F3"   // Blue underline

    // ==================== SIGNALS ====================

    signal tabClicked(int index)

    // ==================== BACKGROUND ====================

    // Top border
    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: 1
        color: Qt.rgba(1, 1, 1, 0.1)
    }

    // Bottom border
    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 1
        color: Qt.rgba(1, 1, 1, 0.1)
    }

    // ==================== TAB ITEMS ====================
    Row {
        id: tabRow
        anchors.fill: parent
        
        Repeater {
            id: tabRepeater
            model: root.tabs

            Item {
                width: root.width / root.tabs.length
                height: root.height

                // Tab Text
                Text {
                    anchors.centerIn: parent
                    text: modelData
                    font.family: "DejaVu Sans"
                    font.pixelSize: 8
                    font.bold: index === root.currentIndex
                    color: index === root.currentIndex ? root.activeColor : root.inactiveColor

                    // RGB565 optimizations
                    renderType: Text.NativeRendering
                    antialiasing: false
                    font.hintingPreference: Font.PreferFullHinting

                    // Smooth color transition
                    Behavior on color {
                        ColorAnimation {
                            duration: 150
                        }
                    }
                }

                // Click area
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        root.currentIndex = index
                        root.tabClicked(index)
                    }
                }
            }
        }
    }

    // ==================== ACTIVE INDICATOR ====================

    Rectangle {
        id: activeIndicator
        anchors {
            bottom: parent.bottom
            bottomMargin: 1
        }
        width: root.width / root.tabs.length
        height: 2
        color: root.indicatorColor

        // Position based on currentIndex
        x: (root.width / root.tabs.length) * root.currentIndex

        // Smooth animation when currentIndex changes
        Behavior on x {
            NumberAnimation {
                duration: 200
                easing.type: Easing.OutCubic
            }
        }
    }
}
