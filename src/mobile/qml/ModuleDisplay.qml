/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

import QtQuick 2.2
import BibleTime 1.0

Rectangle {
    id: moduleDisplay

    property string moduleText: ""

    signal activated()

    border.color: btStyle.inputBorderColor
    border.width: 3
    color: btStyle.textBackgroundColor
    radius:btStyle.pixelsPerMillimeterX
    width: visible? text.width + btStyle.pixelsPerMillimeterX * 2: 0

    Text {
        id: text

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: btStyle.pixelsPerMillimeterX * 1
        anchors.rightMargin: 4
        color: btStyle.textColor
        elide: Text.ElideMiddle
        font.pointSize: btStyle.uiFontPointSize
        text: moduleText
    }

    MouseArea {
        id: moduleMouseArea

        anchors.fill: parent
        onClicked: {
            activated();
        }
    }
}

