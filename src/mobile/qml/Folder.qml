/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

import QtQuick 2.3

Canvas {
    id: canvas

    property color color: "#808080"

    antialiasing: true

    onColorChanged:requestPaint();

    onPaint: {
        var ctx = getContext("2d");
        ctx.save();
        ctx.clearRect(0,0,canvas.width, canvas.height);

        ctx.fillStyle = canvas.color
        ctx.globalAlpha = canvas.alpha

        ctx.beginPath();
        ctx.moveTo(width*0.20,height*0.75);
        ctx.lineTo(width*0.20,height*0.35);
        ctx.lineTo(width*0.25,height*0.35);
        ctx.lineTo(width*0.30,height*0.25);
        ctx.lineTo(width*0.41,height*0.25);
        ctx.lineTo(width*0.46,height*0.35);
        ctx.lineTo(width*0.80,height*0.35);
        ctx.lineTo(width*0.80,height*0.75);
        ctx.lineTo(width*0.20,height*0.75);
        ctx.closePath();
        ctx.fill();

        ctx.restore();
    }
}
