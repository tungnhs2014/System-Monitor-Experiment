/*
 * ============================================
 * File: qml/views/components/CircularGauge.qml
 * Description: Circular gauge
 * ============================================
 */

import QtQuick 2.15

Item {
    id: root

    width: 45
    height: 45

    property real value: 0
    property color startColor: "#9C27B0"
    property color endColor: "#2196F3"
    property bool useGradient: true
    property int strokeWidth: 3
    property int fontSize: 18 

    // Background circle
    Canvas {
        id: backgroundCanvas
        anchors.fill: parent
        antialiasing: true  
        smooth: false  
        renderTarget: Canvas.FramebufferObject 
        renderStrategy: Canvas.Threaded  

        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);

            var centerX = width / 2;
            var centerY = height / 2;
            var radius = (width / 2) - root.strokeWidth;

            ctx.strokeStyle = Qt.rgba(1, 1, 1, 0.1);
            ctx.lineWidth = root.strokeWidth;
            ctx.beginPath();
            ctx.arc(centerX, centerY, radius, 0, 2 * Math.PI);
            ctx.stroke();
        }

        Component.onCompleted: requestPaint()
    }

    // Progress arc
    Canvas {
        id: progressCanvas
        anchors.fill: parent
        antialiasing: true 
        smooth: false  
        renderTarget: Canvas.FramebufferObject  
        renderStrategy: Canvas.Threaded 

        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);

            if (root.value <= 0) return;

            var centerX = width / 2;
            var centerY = height / 2;
            var radius = (width / 2) - root.strokeWidth;

            var startAngle = -Math.PI / 2;
            var endAngle = startAngle + (2 * Math.PI * root.value / 100);

            if (root.useGradient) {
                var gradient = ctx.createLinearGradient(
                    centerX - radius, centerY - radius,
                    centerX + radius, centerY + radius
                );
                gradient.addColorStop(0, root.startColor);
                gradient.addColorStop(1, root.endColor);
                ctx.strokeStyle = gradient;
            } else {
                ctx.strokeStyle = root.endColor;
            }

            ctx.lineWidth = root.strokeWidth;
            ctx.lineCap = "round";
            ctx.beginPath();
            ctx.arc(centerX, centerY, radius, startAngle, endAngle);
            ctx.stroke();
        }
    }
    
    // Center text
    Text {
        anchors.centerIn: parent
        text: Math.round(root.value) + "%"
        font.family: "Roboto"
        font.pixelSize: root.fontSize
        font.bold: true
        color: "#FFFFFF"
        renderType: Text.NativeRendering 
        antialiasing: true  
        smooth: false  
    }

    onValueChanged: {
        progressCanvas.requestPaint();
    }
}
