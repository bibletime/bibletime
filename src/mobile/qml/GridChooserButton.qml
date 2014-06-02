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
import BibleTime 1.0

Rectangle {
    id: button

    property int buttonWidth
    property int buttonHeight
    property int textHeight
    property color textColor
    property color buttonColor
    property color activeButtonColor
    property alias text: buttonText.text

    signal clicked

    width: buttonWidth
    height: buttonHeight
    color: buttonColor
    smooth: true

    border {
        width: 1
        color: btStyle.buttonBorder
    }

    BtStyle {
        id: btStyle
    }

    Text {
        id: buttonText

        width: buttonWidth-8
        anchors.centerIn: parent
        color: button.textColor
        font.pointSize: parent.textHeight
        elide: Text.ElideRight
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        onClicked: button.clicked()
    }
}
