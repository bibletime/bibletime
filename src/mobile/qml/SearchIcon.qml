
import QtQuick 2.3

        Canvas {
            id: canvas
            width:  160
            height: 160
            antialiasing: true

            property color strokeStyle:  "#888888"
            property color fillStyle: "#000000"

            onStrokeStyleChanged:requestPaint();

            onPaint: {
                var ctx = getContext("2d");
                ctx.save();
                ctx.clearRect(0,0,canvas.width, canvas.height);

                ctx.strokeStyle = canvas.strokeStyle;
                ctx.fillStyle = canvas.fillStyle
                ctx.globalAlpha = canvas.alpha

                ctx.lineWidth = width * 0.08
                ctx.beginPath();
                ctx.arc(width*.48,height*0.42,width*0.2,0,Math.PI*2,true);
                ctx.stroke();

                ctx.beginPath();
                ctx.moveTo(width*0.59,height*0.56);
                ctx.lineTo(width*0.78,height*0.82);
                ctx.stroke();

                ctx.restore();
            }
        }
