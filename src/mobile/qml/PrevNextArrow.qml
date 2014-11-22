
import QtQuick 2.3

Item {
    id:container

    property color background: "#000000"
    property bool show: false
    property bool prev: false
    property color textColor: Qt.rgba(1,1,1, 1)
    onTextColorChanged: canvas.requestPaint();

    signal clicked

    Rectangle {
        anchors.fill: parent
        color: background
        opacity: show ? 0 : 0.7
        z: 1
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
                container.clicked();
        }
    }
    Column {
        spacing: 1
        anchors.fill: parent
        anchors.topMargin: 0

        Canvas {
            id: canvas
            width:  parent.width-2
            height: parent.height-2
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
                ctx.lineWidth = width * 0.09;

                var arrowhead = width*0.8;
                var arrowtail = width*0.2;
                if (prev) {
                    arrowhead = width*0.2;
                    arrowtail = width*0.8;
                }

                ctx.beginPath();
                ctx.moveTo(arrowtail,height*0.5);
                ctx.lineTo(arrowhead,height*0.5);
                ctx.stroke();

                ctx.beginPath();
                ctx.moveTo(width*0.5,arrowtail);
                ctx.lineTo(arrowhead,height*0.5);
                ctx.lineTo(width*0.5,arrowhead);
                ctx.stroke();

                ctx.restore();
            }
        }
    }
}
