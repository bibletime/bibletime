
import QtQuick 2.3

        Canvas {
            id: canvas
            width:  260
            height: 260
            antialiasing: true

            property color color: "#808080"

            onColorChanged:requestPaint();

            onPaint: {
                var ctx = getContext("2d");
                ctx.save();
                ctx.clearRect(0,0,canvas.width, canvas.height);

                ctx.fillStyle = canvas.color
                ctx.globalAlpha = canvas.alpha

                ctx.beginPath();
                ctx.moveTo(width*0.2,height*0.8);
                ctx.lineTo(width*0.2,height*0.2);
                ctx.lineTo(width*0.8,height*0.5);
                ctx.closePath();
                ctx.fill();

                ctx.restore();
            }
        }
