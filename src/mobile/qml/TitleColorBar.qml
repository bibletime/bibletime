/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2015 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

import QtQuick 2.2
import BibleTime 1.0

Rectangle {
    id: titleBar

    property string title: ""
    property int barHeight: {
        var pixel = btStyle.pixelsPerMillimeterY * 6;
        var uiFont = btStyle.uiFontPointSize * 2;
        return Math.max(pixel, uiFont);
    }

    color: btStyle.toolbarColor
    border.color: "black"
    border.width: 1
    height: barHeight

    Text {
        id: title
        color: btStyle.toolbarTextColor
        font.pointSize: btStyle.uiFontPointSize
        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: titleBar.title
    }

    BtStyle {
        id: btStyle
    }
}

