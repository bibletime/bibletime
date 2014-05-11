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
    id: question

    property alias text: questionText.text
    property bool answer
    property color background

    signal finished();

    anchors.fill: parent
    color: background
    visible: false
    width: parent.width

    Text {
        id: questionText
        height: contentHeight
        width: parent.width
        anchors.centerIn: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        font.pointSize: btStyle.uiFontPointSize
        anchors.margins: 60
    }

    Grid {
        id: buttons

        spacing: btStyle.pixelsPerMillimeterY * 4
        columns: 2
        anchors.top: questionText.bottom
        anchors.topMargin: 50
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 50

        Rectangle {
            height: btStyle.pixelsPerMillimeterY * 7
            width: btStyle.pixelsPerMillimeterY * 25
            border.color: "black"
            border.width: 4

            Text {
                text: QT_TR_NOOP("Yes")
                anchors.centerIn: parent
                font.pointSize: btStyle.uiFontPointSize
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    question.visible = false;
                    answer = true;
                    finished();
                }
            }
        }

        Rectangle {
            height: btStyle.pixelsPerMillimeterY * 7
            width: btStyle.pixelsPerMillimeterY * 25
            border.color: "black"
            border.width: 4

            Text {
                text: QT_TR_NOOP("No")
                anchors.centerIn: parent
                font.pointSize: btStyle.uiFontPointSize
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    answer = false;
                    question.visible = false;
                    finished();
                }
            }
        }
    }

}
