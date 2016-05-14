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
    id: menuButton


    property color foreground: "#444444"
    property color background: "#cccccc"

    signal buttonClicked


    color: background

    Column {
        id: column
        property real ppmy: btStyle.pixelsPerMillimeterY
        property real buttonWidth: {
            var w = column.ppmy * 0.7;
            return Math.max(3, w);
        }

        spacing: menuButton.height*0.1

        Rectangle {
            color: foreground
            width: column.buttonWidth
            height:column.buttonWidth
        }
        Rectangle {
            color: foreground
            width: column.buttonWidth
            height:column.buttonWidth
        }
        Rectangle {
            color: foreground
            width: column.buttonWidth
            height:column.buttonWidth
        }

        anchors.centerIn: parent
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            menuButton.buttonClicked()
        }
    }
}
