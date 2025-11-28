/*
 * ============================================
 * File: qml/Main.qml (UPDATED - Full Navigation)
 * Description: Root window with complete StackView navigation
 * ============================================
 *
 * PAGES:
 *   1. Dashboard (initial)
 *   2. CpuDetail
 *   3. MemoryDetail
 *   4. StorageDetail
 *   5. NetworkDetail
 *   6. Settings
 */

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import "views/pages"
import "views/components"

Window {
    id: root

    width: 320
    height: 240

    visible: true
    title: "ILI9341 System Monitor"

    // Frameless window for embedded
    flags: Qt.FramelessWindowHint

    color: "#0F1419"

    // Performance optimizations for embedded
    Component.onCompleted: {
        // Disable unnecessary visual updates
        contentItem.smooth = false
        contentItem.antialiasing = false
    }

    // ==================== NAVIGATION STACK ====================
    StackView {
        id: stackView
        anchors.fill: parent

        // Initial page
        initialItem: dashboardComponent

        // Dashboard component
        Component {
            id: dashboardComponent
            
            Dashboard {

            }
        }

        // Push transition (slide in from right)
        pushEnter: Transition {
            PropertyAnimation {
                property: "x"
                from: root.width
                to: 0
                duration: 250
                easing.type: Easing.OutCubic
            }
            PropertyAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 250
            }
        }
        
        // Push exit (current page fades out)
        pushExit: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: 250
            }
        }
        
        // Pop transition (slide out to right)
        popExit: Transition {
            PropertyAnimation {
                property: "x"
                from: 0
                to: root.width
                duration: 250
                easing.type: Easing.InCubic
            }
            PropertyAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: 250
            }
        }
        
        // Pop enter (previous page fades in)
        popEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 250
            }
        }
    }

    // ==================== CONNECTIONS FOR NAVIGATION ====================

    // Listen for navigation events from current page
    Connections {
        target: stackView.currentItem
        ignoreUnknownSignals: true  // Ignore signals that don't exist on current page

        // Handle back button clicks from DetailHeader
        function onBackRequested() {
            if (stackView.depth > 1) {
                stackView.pop()
            }
        }

        // Handle settings button clicks from DetailHeader
        function onSettingsRequested() {
            // If already on Settings, do nothing
            if (stackView.currentItem.objectName === "settings") {
                return
            }
            stackView.push(settingsComponent)
        }

        // Handle bottom nav clicks
        function onNavigationRequested(index) {
            // Pop to dashboard first
            stackView.pop(null)

            // Then push desired page (if not dashboard)
            if (index === 1) stackView.push(cpuDetailComponent)
            if (index === 2) stackView.push(memoryDetailComponent)
            if (index === 3) stackView.push(storageDetailComponent)
            if (index === 4) stackView.push(networkDetailComponent)
        }
    }
    
    // ==================== PAGE COMPONENTS ====================
    
    Component {
        id: cpuDetailComponent
        CpuDetail {
            objectName: "cpuDetail"
        }
    }
    
    Component {
        id: memoryDetailComponent
        MemoryDetail {
            objectName: "memoryDetail"
        }
    }
    
    Component {
        id: storageDetailComponent
        StorageDetail {
            objectName: "storageDetail"
        }
    }
    
    Component {
        id: networkDetailComponent
        NetworkDetail {
            objectName: "networkDetail"
        }
    }
    
    Component {
        id: settingsComponent
        Settings {
            objectName: "settings"
        }
    }

    // ==================== KEYBOARD SHORTCUTS (Desktop Testing) ====================
    
    Item {
        id: keyboardHandler
        focus: true
        
        Keys.onPressed: function(event) {
            // Esc or Backspace = Go back
            if (event.key === Qt.Key_Escape || event.key === Qt.Key_Backspace) {
                if (stackView.depth > 1) {
                    stackView.pop()
                }
                event.accepted = true
            }
            
            // Number keys for quick navigation (Desktop testing)
            else if (event.key === Qt.Key_0) {
                stackView.pop(null) // Dashboard
                event.accepted = true
            }
            else if (event.key === Qt.Key_1) {
                stackView.pop(null)
                stackView.push(cpuDetailComponent)
                event.accepted = true
            }
            else if (event.key === Qt.Key_2) {
                stackView.pop(null)
                stackView.push(memoryDetailComponent)
                event.accepted = true
            }
            else if (event.key === Qt.Key_3) {
                stackView.pop(null)
                stackView.push(storageDetailComponent)
                event.accepted = true
            }
            else if (event.key === Qt.Key_4) {
                stackView.pop(null)
                stackView.push(networkDetailComponent)
                event.accepted = true
            }
            else if (event.key === Qt.Key_9) {
                stackView.push(settingsComponent)
                event.accepted = true
            }
        }
    }
    
    // ==================== DEBUG INFO (Optional) ====================
    
    Text {
        id: debugInfo
        visible: false  // Set to true for debugging
        anchors {
            top: parent.top
            left: parent.left
            margins: 4
        }
        text: "Stack: " + stackView.depth + " | Page: " + 
              (stackView.currentItem ? stackView.currentItem.objectName : "none")
        font.pixelSize: 7
        color: "#FFFFFF"
        z: 1000
        
        renderType: Text.NativeRendering
        antialiasing: false
        font.hintingPreference: Font.PreferFullHinting
    }
}
