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
import QtQuick.Controls 1.2
import BibleTime 1.0

Rectangle {
    id: question

    property alias text: questionText.text
    property bool answer
    property color background

    signal finished();

    anchors.fill: parent
    color: btStyle.textBackgroundColor
    visible: false
    width: parent.width

    Image {
        id: logo

        width: btStyle.pixelsPerMillimeterX*12
        height: width
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: width * 0.2
        anchors.leftMargin: width * 0.2
        source: "qrc:/share/bibletime/icons/bibletime.svg"
    }

    Text {
        id: questionTitle

        color: btStyle.textColor
        font.pointSize: btStyle.uiFontPointSize
        text: "BibleTime"
        anchors.left: logo.right
        anchors.verticalCenter: logo.verticalCenter
        anchors.leftMargin: logo.width * 0.2
    }

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
        color: btStyle.textColor
    }

    Text {
        id: dummyTextForHeight
        text: "x"
        font.pointSize: btStyle.uiFontPointSize
        visible: false
    }

    Grid {
        id: buttons

        spacing: btStyle.pixelsPerMillimeterY * 4
        columns: 2
        anchors.top: questionText.bottom
        anchors.topMargin: 50
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 50

        Action {
            id: yesAction
            text: qsTranslate("Question","Yes")
            onTriggered: {
                question.visible = false;
                answer = true;
                finished();
            }
        }

        Button {
            id: yesButton
            width: btStyle.pixelsPerMillimeterY * 25
            height: dummyTextForHeight.height*1.7
            action: yesAction
            style: BtButtonStyle {
            }
        }

        Action {
            id: noAction
            text: qsTranslate("Question","No")
            onTriggered: {
                question.visible = false;
                answer = false;
                finished();
            }
        }

        Button {
            id: noButton
            width: btStyle.pixelsPerMillimeterY * 25
            height: dummyTextForHeight.height*1.7
            action: noAction
            style: BtButtonStyle {
            }
        }

    }

}
