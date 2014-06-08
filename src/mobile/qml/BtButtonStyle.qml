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

import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import BibleTime 1.0

ButtonStyle {

    background: Rectangle {
        border.width: 2
        border.color: "black"
        radius: btStyle.pixelsPerMillimeterX
        gradient: Gradient {
            GradientStop { position: 0 ; color: control.pressed ? "#ccc" : "#fff" }
            GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
        }
    }

    label: Text {
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: btStyle.uiFontPointSize
        color: "black"
        text: control.text
    }
}
