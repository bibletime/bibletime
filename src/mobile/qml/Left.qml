
import QtQuick 2.3

Item {
    id:container

    property color background: "#000000"
    property bool show: false
    property color borderColor: "black"
    property color textColor: Qt.rgba(1,1,1, 1)
    onTextColorChanged: canvas.requestPaint();

    signal clicked

//    Rectangle {
//        anchors.fill: parent
//        color: background
//        opacity: show ? 0 : 0.7
//        z: 2
//    }

//    Rectangle {
//        anchors.fill: parent
//        color: Qt.rgba(0,0,0,0)
//        border.width: 1
//        border.color: borderColor
//        z: 1
//    }

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
                ctx.moveTo(width*0.9,height*0.8);
                ctx.lineTo(width*0.2,height*0.5);
                ctx.lineTo(width*0.9,height*0.2);
                ctx.stroke();

                ctx.restore();
            }
        }
    }
}
