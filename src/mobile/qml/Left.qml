
import QtQuick 2.3

Item {
    id:container

    property color background: "#000000"
    property bool show: false
    property color borderColor: "black"
    property color textColor: Qt.rgba(1,1,1, 1)
    onTextColorChanged: canvas.requestPaint();

    signal clicked

    Column {
        spacing: 1
        anchors.fill: parent
        anchors.topMargin: 0

        Canvas {
            id: canvas
            width:  parent.width
            height: parent.height
            antialiasing: true

            property color bright: Qt.rgba(255, 255, 255, 0.0)

            onPaint: {
                var ctx = getContext("2d");
                ctx.save();
                ctx.globalAlpha = canvas.alpha
                ctx.clearRect(0,0,canvas.width, canvas.height);

                ctx.lineCap = "round";
                ctx.lineJoin = "round";
                ctx.strokeStyle = container.textColor;
                ctx.lineWidth = width * 0.16;

                ctx.beginPath();
                ctx.moveTo(width*0.9,height*0.7);
                ctx.lineTo(width*0.4,height*0.5);
                ctx.lineTo(width*0.9,height*0.3);
                ctx.stroke();

                ctx.restore();
            }
        }
    }
}
