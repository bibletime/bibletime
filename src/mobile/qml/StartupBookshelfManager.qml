
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
    id: startupBookshelfManager

    signal bookshelfRequested()

    color: "white"

    Text {
        width: parent.width
        anchors.fill: parent
        text: qsTr("There are currently no works installed. Please click on the Ok button below to use the Bookshelf Manager to install new works.")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        font.pointSize: btStyle.uiFontPointSize
        anchors.margins: 30
     }

    Grid {
        id: buttons

        spacing: btStyle.pixelsPerMillimeterY * 4
        columns: 2
        anchors.bottom: startupBookshelfManager.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 50

        Rectangle {
            height: btStyle.pixelsPerMillimeterY * 7
            width: btStyle.pixelsPerMillimeterY * 25
            border.color: "black"
            border.width: 4

            Text {
                text: qsTr("Ok")
                anchors.centerIn: parent
                font.pointSize: btStyle.uiFontPointSize
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    startupBookshelfManager.visible = false;
                    startupBookshelfManager.bookshelfRequested()
                }
            }
        }

        Rectangle {
            height: btStyle.pixelsPerMillimeterY * 7
            width: btStyle.pixelsPerMillimeterY * 25
            border.color: "black"
            border.width: 4

            Text {
                text: qsTr("Cancel")
                anchors.centerIn: parent
                font.pointSize: btStyle.uiFontPointSize
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    startupBookshelfManager.visible = false;
                }
            }
        }
    }

    BtStyle {
        id: btStyle
    }
}
