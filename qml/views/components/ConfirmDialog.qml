/**
 * ============================================================================
 * File: qml/views/components/ConfirmDialog.qml
 * Description: Modal confirmation dialog for dangerous actions
 * ============================================================================
 */

import QtQuick 2.15

Rectangle {
    id: root
    
    anchors.fill: parent
    color: Qt.rgba(0, 0, 0, 0.7)
    visible: false
    z: 1000
    
    // Properties
    property string title: "Confirm"
    property string message: "Are you sure?"
    property string confirmText: "Confirm"
    property string cancelText: "Cancel"
    property color confirmColor: "#F44336"
    
    // Signals
    signal accepted()
    signal rejected()
    
    // Show/hide methods
    function show() {
        visible = true
    }
    
    function hide() {
        visible = false
    }
    
    // Close on background click
    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.hide()
            root.rejected()
        }
    }
    
    // Dialog box
    Rectangle {
        id: dialogBox
        width: 260
        height: 130
        anchors.centerIn: parent
        color: "#1E2539"
        radius: 8
        border.width: 1
        border.color: Qt.rgba(1, 1, 1, 0.2)
        
        // Prevent click through
        MouseArea {
            anchors.fill: parent
            onClicked: { } // Consume click
        }
        
        Column {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12
            
            // Title
            Text {
                width: parent.width
                text: root.title
                font.family: "DejaVu Sans"
                font.pixelSize: 14
                font.bold: true
                font.hintingPreference: Font.PreferFullHinting
                color: "#FFFFFF"
                horizontalAlignment: Text.AlignHCenter
                renderType: Text.NativeRendering
                antialiasing: false
            }
            
            // Message
            Text {
                width: parent.width
                text: root.message
                font.family: "DejaVu Sans"
                font.pixelSize: 10
                font.hintingPreference: Font.PreferFullHinting
                color: "#B0B8C8"
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                renderType: Text.NativeRendering
                antialiasing: false
            }
            
            // Spacer
            Item {
                width: parent.width
                height: 8
            }
            
            // Buttons
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 16
                
                // Cancel button
                Rectangle {
                    width: 90
                    height: 32
                    radius: 4
                    color: cancelArea.pressed ? "#3A4558" : "#2A3548"
                    
                    Text {
                        anchors.centerIn: parent
                        text: root.cancelText
                        font.family: "DejaVu Sans"
                        font.pixelSize: 11
                        font.bold: true
                        color: "#FFFFFF"
                        renderType: Text.NativeRendering
                        antialiasing: false
                    }
                    
                    MouseArea {
                        id: cancelArea
                        anchors.fill: parent
                        onClicked: {
                            root.hide()
                            root.rejected()
                        }
                    }
                }
                
                // Confirm button
                Rectangle {
                    width: 90
                    height: 32
                    radius: 4
                    color: confirmArea.pressed ? Qt.darker(root.confirmColor, 1.2) : root.confirmColor
                    
                    Text {
                        anchors.centerIn: parent
                        text: root.confirmText
                        font.family: "DejaVu Sans"
                        font.pixelSize: 11
                        font.bold: true
                        color: "#FFFFFF"
                        renderType: Text.NativeRendering
                        antialiasing: false
                    }
                    
                    MouseArea {
                        id: confirmArea
                        anchors.fill: parent
                        onClicked: {
                            root.hide()
                            root.accepted()
                        }
                    }
                }
            }
        }
    }
}
