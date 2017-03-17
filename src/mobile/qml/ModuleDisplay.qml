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

    width: visible? text.width + 30: 0
    radius:btStyle.pixelsPerMillimeterX
    color: btStyle.textBackgroundColor
    border.color: btStyle.toolbarTextColor
    border.width: 2

    Text {
        id: text

        anchors.centerIn: parent
        anchors.leftMargin: 4
        anchors.rightMargin: 4
        font.pointSize: btStyle.uiFontPointSize
        elide: Text.ElideMiddle
        color: btStyle.textColor
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

