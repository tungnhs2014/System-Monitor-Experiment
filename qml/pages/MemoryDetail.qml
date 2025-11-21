/*
 * ============================================
 * File: qml/pages/MemoryDetail.qml
 * Description: Memory detail page with large circular gauge
 * ============================================
 *
 */

import QtQuick 2.15
import "../components"

Rectangle {
    id: root

    // ==================== PROPERTIES ====================

    width: 320
    height: 240
    color: "#0F1419"    // Dark background matching Dashboard

    // ==================== MOCK DATA ====================
    // TODO: Replace with systemInfo.xxx when backend ready

    nPia: {ìif (property int mockRamUsage: 73            // RAM usage percentage
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

        // Navigation signals (connect to StackView when integrated)
        onBackClicked: {
            console.log("Back to Dashboard")
            // TODO: stackView.pop()
        }

        onsettingsClicked: {
            console.log("Open Settings")
            // TODO: stackView.push("Settings.qml")
        }
    }
}