/*
 * ============================================
 * File: qml/views/components/InputField.qml
 * Description: Number input field component
 * ============================================
 */

import QtQuick 2.15

Rectangle {
    id: root

    // ==================== PROPERTIES ====================

    width: 60
    height: 24
    radius: 4
    color: "#1E2539"
    border.width: 1
    border.color: textInput.activeFocus ? "#2196F3" : Qt.rgba(1, 1, 1, 0.2)
    
    property int value: 0
    property int minValue: 0
    property int maxValue: 100
    property string suffix: ""
    property bool enabled: true

    // ==================== SIGNALS ====================

    signal editingFinished()

    // ==================== INPUT FIELD ====================
    
    TextInput {
        id: textInput 
        anchors {
            fill: parent
            leftMargin: 6
            rightMargin: suffixText.visible ? suffixText.width + 6 : 6
            topMargin: 4
            bottomMargin: 4
        }
        text: root.value.toString()
        font.family: "DejaVu Sans"
        font.pixelSize: 10
        color: root.enabled ? "#FFFFFF" : "#B0B8C8"

        // RGB565 optimizations
        renderType: Text.NativeRendering
        antialiasing: false
        font.hintingPreference: Font.PreferFullHinting

        // Only allow numbers
        validator: IntValidator {
            bottom: root.minValue
            top: root.maxValue
        }

        // Horizontal alignment
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter

        // Selection color
        selectionColor: "#2196F3"
        selectedTextColor: "#FFFFFF"
        
        // Enable/disable
        enabled: root.enabled
        readOnly: !root.enabled

        // Handle input
        onTextChanged: {
            if (text === "") return

            var newValue = parseInt(text)

            // Validate range
            if (newValue < root.minValue) {
                newValue = root.minValue
            }
            else if (newValue > root.maxValue) {
                newValue = root.maxValue
            }
            
            // Update value if changed (auto-emits valueChanged signal)
            if (newValue != root.value) {
                root.value = newValue
            }
        }

        onEditingFinished: {
            // Ensure text matches value (in case of invalid input)
            text = root.value.toString()
            root.editingFinished()
        }

        // Focus handling
        onActiveFocusChanged: {
            if (activeFocus) {
                selectAll()
            }
        }
    }

    // ==================== SUFFIX TEXT ====================

    Text {
        id: suffixText
        visible: root.suffix !== ""
        anchors {
            right: parent.right
            rightMargin: 6
            verticalCenter: parent.verticalCenter
        }
        text: root.suffix
        font.family: "DejaVu Sans"
        font.pixelSize: 9
        color: "#B0B8C8"

        // RGB565 optimizations
        renderType: Text.NativeRendering
        antialiasing: false
        font.hintingPreference: Font.PreferFullHinting
    }

    // ==================== PLACEHOLDER (when empty) ====================

    Text {
        anchors {
            left: parent.left
            leftMargin: 6
            verticalCenter: parent.verticalCenter
        }
        visible: textInput.text === "" && !textInput.activeFocus
        text: root.minValue.toString()
        font.family: "DejaVu Sans"
        font.pixelSize: 10
        color: Qt.rgba(1, 1, 1, 0.3)

        renderType: Text.NativeRendering
        antialiasing: false
        font.hintingPreference: Font.PreferFullHinting
    }

    // ==================== MOUSE AREA (for click to focus) ====================

    MouseArea {
        anchors.fill: parent
        enabled: root.enabled
        cursorShape: root.enabled ? Qt.IBeamCursor : Qt.ForbiddenCursor

        onClicked: {
            textInput.forceActiveFocus()
        }
    }

    // ==================== SMOOTH BORDER TRANSITION ====================

    Behavior on border.color {
        ColorAnimation {
            duration: 150
        }
    }
}