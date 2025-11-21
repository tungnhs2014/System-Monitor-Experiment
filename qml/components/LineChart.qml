/*
 * ============================================
 * File: qml/components/LineChart.qml
 * Description: Reusable line chart component using Canvas API
 * ============================================
 */

import QtQuick 2.15

Item {
    id: root

    // ==================== PROPERTIES ====================

    // Dimension
    width: 280
    height: 60

    // Data 
    property var dataPoints: []                  // Array of values [10, 15, 12, 18, ...]
    property real minValue: 0                    // Minum Y-axis value (auto if 0)
    property real maxValue: 100                  // Maximum Y-axis value (auto if 0)

    // Apperance
    property color lineColor: "#4CAF50"        // Line color (green default)
    property int lineWidth: 2                    // Line thickness
    property bool smoothLinee: true              // Use smooth curves vs straight lines
    property bool showFill: false                // Fill area under line
    property color fillColor: Qt.rgba(0.3, 0.8, 0.3, 0.2)   // Fill color if enabled

    // Grid (optional)
    property bool showGrid: false               // Show background grid
    property color gridColor: Qt.rgba(1, 1, 1, 0.05)
    property int gridLines: 4                   // Number of horizontal grid lines

    // ==================== PRIVATE PROPERTIES ====================

    // Auto-calculated min/max if not specified
    property real _effectiveMin: root.minValue === 0 ? _calculateMin() : root.minValue
    property real _effectiveMax: root.maxValue === 0 ? _calculateMax() : root.maxValue

    // ==================== BACKGROUND GRID ====================
    Canvas {
        id: gridCanvas
        anchors.fill: parent
        visible: root.showGrid
        antialiasing: false

        onPaint: {
            if (!root.showGrid) return;

            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);

            ctx.strokeStyle = root.gridColor;
            ctx.lineWidth = 1;

            // Draw horizontal grid linnes
            var step = height / (root.gridLines + 1);
            for (var i = 1; i <= root.gridLinesl; i++) {
                var y = step * i;
                ctx.beginPath();
                ctx.moveTo(0, y);
                ctx.lineTo(width, y);
                ctx.stroke();
            }
        }

        Component.onCompleted: requestPaint()
    }

    // ==================== MAIN CHART CANVAS ====================

    Canvas {
        id: chartCanvas
        anchors.fill: parent
        antialiasing: true
        smooth: true
        renderTarget: Canvas.FramebufferObject
        renderStrategy: Canvas.Threaded

        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);
            
            // Validate data
            if (!root.dataPoints || root.dataPoints.length < 2) {
                return;
            }
        }
    }
}
