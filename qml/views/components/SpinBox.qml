/**
 * ============================================================================
 * File: qml/views/components/SpinBox.qml
 * Description: Touch-friendly numeric input with +/- buttons
 * Replaces InputField for keyboard-less devices
 * ============================================================================
 */

import QtQuick 2.15

Item {
    id: root
    
    width: 140  
    height: 44 
    
    // Properties
    property int value: 70
    property int minValue: 0
    property int maxValue: 100
    property int step: 5
    property string suffix: "%"
    property bool enabled: true

    // Note: valueChanged signal is automatically created by the 'value' property
    
    Row {
        anchors.fill: parent
        spacing: 0
        
        // ==================== MINUS BUTTON ====================
        Rectangle {
            id: minusBtn
            width: 44   
            height: 44  
            radius: 4
            color: "#1E2539"
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.2)
            
            Rectangle {
                anchors.fill: parent
                radius: 4
                color: minusArea.pressed ? "#2196F3" : 
                       (minusArea.containsMouse ? "#2A3548" : "transparent")
            }
            
            Text {
                anchors.centerIn: parent
                text: "−"
                font.pixelSize: 20 
                font.bold: true
                font.family: "DejaVu Sans"
                color: root.value > root.minValue && root.enabled ? "#FFFFFF" : "#4A5568"
                renderType: Text.NativeRendering
                antialiasing: false
            }
            
            MouseArea {
                id: minusArea
                anchors.fill: parent
                hoverEnabled: true
                enabled: root.enabled && root.value > root.minValue
                
                onClicked: {
                    if (root.value - root.step >= root.minValue) {
                        root.value -= root.step
                    }
                }
                
                // Long press for rapid decrease
                onPressAndHold: rapidTimer.direction = -1
                onReleased: rapidTimer.stop()
            }
        }
        
        // ==================== VALUE DISPLAY ====================
        Rectangle {
            width: 52  // Fixed width for value
            height: 44
            radius: 4
            color: "#1E2539"
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.2)
            
            Text {
                anchors.centerIn: parent
                text: root.value + root.suffix
                font.family: "DejaVu Sans"
                font.pixelSize: 13 
                font.bold: true
                font.hintingPreference: Font.PreferFullHinting
                color: root.enabled ? "#FFFFFF" : "#4A5568"
                renderType: Text.NativeRendering
                antialiasing: false
            }
        }
        
        // ==================== PLUS BUTTON ====================
        Rectangle {
            id: plusBtn
            width: 44   
            height: 44 
            radius: 4
            color: "#1E2539"
            border.width: 1
            border.color: Qt.rgba(1, 1, 1, 0.2)
            
            Rectangle {
                anchors.fill: parent
                radius: 4
                color: plusArea.pressed ? "#2196F3" : 
                       (plusArea.containsMouse ? "#2A3548" : "transparent")
            }
            
            Text {
                anchors.centerIn: parent
                text: "+"
                font.pixelSize: 20 
                font.bold: true
                font.family: "DejaVu Sans"
                color: root.value < root.maxValue && root.enabled ? "#FFFFFF" : "#4A5568"
                renderType: Text.NativeRendering
                antialiasing: false
            }
            
            MouseArea {
                id: plusArea
                anchors.fill: parent
                hoverEnabled: true
                enabled: root.enabled && root.value < root.maxValue
                
                onClicked: {
                    if (root.value + root.step <= root.maxValue) {
                        root.value += root.step
                    }
                }
                
                // Long press for rapid increase
                onPressAndHold: rapidTimer.direction = 1
                onReleased: rapidTimer.stop()
            }
        }
    }
    
    // Timer for rapid value change on long press
    Timer {
        id: rapidTimer
        property int direction: 0
        interval: 100
        repeat: true
        running: direction !== 0
        
        onTriggered: {
            var newValue = root.value + (root.step * direction)
            if (newValue >= root.minValue && newValue <= root.maxValue) {
                root.value = newValue
            } else {
                stop()
            }
        }
    }
}
