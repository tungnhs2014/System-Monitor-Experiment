/*
 * ============================================
 * File: qml/views/components/LineChart.qml
 * Description: Reusable line chart component using Canvas API
 * ============================================
 */

import QtQuick 2.15

Item {
    id: root

    // ==================== PROPERTIES ====================
    
    // Dimensions
    width: 280
    height: 60
    
    // Data
    property var dataPoints: []           // Array of values [10, 15, 12, 18, ...]
    property real minValue: 0             // Minimum Y-axis value (auto if 0)
    property real maxValue: 100           // Maximum Y-axis value (auto if 0)
    
    // Appearance
    property color lineColor: "#4CAF50"   // Line color (green default)
    property int lineWidth: 2             // Line thickness
    property bool smoothLine: true        // Use smooth curves vs straight lines
    property bool showFill: false         // Fill area under line
    property color fillColor: Qt.rgba(0.3, 0.8, 0.3, 0.2)  // Fill color if enabled
    
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
            
            // Draw horizontal grid lines
            var step = height / (root.gridLines + 1);
            for (var i = 1; i <= root.gridLines; i++) {
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
                return;  // Need at least 2 points to draw line
            }

            var points = root.dataPoints;
            var numPoints = points.length;

            // Calculate effective range
            var minVal = root._effectiveMin;
            var maxVal = root._effectiveMax;
            var range = maxVal - minVal;

            if (range === 0) {
                range = 1; // Avoid division by zero
            }

            // Calculate X and Y coordinates for each data point
            var coords = [];
            for (var i = 0; i < numPoints; i++) {
                var x = (width / (numPoints - 1)) * i;
                var normalizedValue = (points[i] - minVal) / range;
                var y = height - (normalizedValue * height);  // Invert Y (canvas Y grows downward)
                coords.push({x: x, y: y});
            }

            // ==================== FILL AREA (if enabled) ====================
            if (root.showFill) {
                ctx.fillStyle = root.fillColor;
                ctx.beginPath();
                ctx.moveTo(coords[0].x, height);  // Start at bottom-left
                
                // Draw line to first point
                ctx.lineTo(coords[0].x, coords[0].y);

                // Draw through all points
                if (root.smoothLine) {
                    _drawSmoothPath(ctx, coords, false);
                } else {
                    for (var j = 1; j < coords.length; j++) {
                        ctx.lineTo(coords[j].x, coords[j].y);
                    }
                }
                
                // Close path at bottom-right     
                ctx.lineTo(coords[coords.length - 1].x, height);
                ctx.closePath(); 
                ctx.fill();          
            }

            // ==================== DRAW LINE ====================
            
            ctx.strokeStyle = root.lineColor;
            ctx.lineWidth = root.lineWidth;
            ctx.lineCap = "round";
            ctx.lineJoin = "round";

            ctx.beginPath();
            ctx.moveTo(coords[0].x, coords[0].y);

            if (root.smoothLine) {
                _drawSmoothPath(ctx, coords, true);
            } else {
                // Straight lines between points
                for (var k = 1; k < coords.length; k++) {
                    ctx.lineTo(coords[k].x, coords[k].y);
                }
            }

            ctx.stroke();
        }

        // ==================== SMOOTH CURVE DRAWING ====================
        
        function _drawSmoothPath(ctx, coords, isStroke) {
            // Use quadratic curves for smooth line
            // Algorithm: Midpoint between each pair of points becomes control point

            for (var i = 1; i < coords.length - 1; i++) {
                var xc = (coords[i].x + coords[i + 1].x) / 2;
                var yc = (coords[i].y + coords[i + 1].y) / 2;
                ctx.quadraticCurveTo(coords[i].x, coords[i].y, xc, yc);
            }
            
            // Last segment to final point
            var lastIdx = coords.length - 1;
            ctx.quadraticCurveTo(
                coords[lastIdx].x,
                coords[lastIdx].y,
                coords[lastIdx].x,
                coords[lastIdx].y
            );
        }
    }

    // ==================== AUTO-SCALING FUNCTIONS ====================
    function _calculateMin() {
        if (!root.dataPoints || root.dataPoints.length === 0) return 0;

        var min = root.dataPoints[0];
        for (var i = 1; i < root.dataPoints.length; i++) {
            if (root.dataPoints[i] < min) {
                min = root.dataPoints[i];
            }
        }

        // Add 10% padding below
        return Math.floor(min * 0.9);
    }

    function _calculateMax() {
        if (!root.dataPoints || root.dataPoints.length === 0) return 100;

        var max = root.dataPoints[0];
        for (var i = 1; i < root.dataPoints.length; i++) {
            if (root.dataPoints[i] > max) {
                max = root.dataPoints[i];
            }
        }
        // Add 10% padding above
        return Math.ceil(max * 1.1);
    }

    // ==================== DATA CHANGE HANDLER ====================

    onDataPointsChanged: {
        chartCanvas.requestPaint();
    }
    
    onLineColorChanged: {
        chartCanvas.requestPaint();
    }

    onSmoothLineChanged: {
        chartCanvas.requestPaint();
    }

    onShowFillChanged: {
        chartCanvas.requestPaint();
    }
    
    onMinValueChanged: {
        chartCanvas.requestPaint();
    }
    
    onMaxValueChanged: {
        chartCanvas.requestPaint();
    }

    // ==================== INITIALIZATION ====================

    Component.onCompleted: {
        chartCanvas.requestPaint();
        if (root.showGrid) {
            gridCanvas.requestPaint();
        }
    }

}
