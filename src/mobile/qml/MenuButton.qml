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

import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: menuButton

    signal buttonClicked

    color: btStyle.toolbarColor

    BtStyle {
        id: btStyle
    }

    Column {
        id: column
        property real ppmy: btStyle.pixelsPerMillimeterY

        spacing: column.ppmy * 0.6

        Rectangle {
            color: "black";
            width: column.ppmy * 0.8
            height:column.ppmy * 0.8
        }
        Rectangle {
            color: "black";
            width:column.ppmy * 0.7
            height:column.ppmy * 0.7
        }
        Rectangle {
            color: "black";
            width:column.ppmy * 0.7
            height:column.ppmy * 0.7
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
