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
    id: continueDialog

    property alias text: continueText.text
    property bool answer
    property color background

    signal finished();

    anchors.fill: parent
    color: btStyle.textBackgroundColor
    visible: false
    width: parent.width

    Rectangle {
        id: continueTitleBar
        color: btStyle.toolbarColor
        width: parent.width
        height: {
            var pixel = btStyle.pixelsPerMillimeterY * 7.5;
            var uiFont = btStyle.uiFontPointSize * 4.4;
            var mix = pixel * 0.7 + uiFont * 0.3;
            return Math.max(pixel, mix);
        }

        Image {
            id: logo

            width: parent.height - 10
            height: parent.height - 10
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: width * 0.1
            anchors.leftMargin: width * 0.1
            source: "qrc:/share/bibletime/icons/bibletime.svg"
        }


        Text {
            id: continueTitle

            color: btStyle.textColor
            font.pointSize: btStyle.uiFontPointSize * 1.1
            text: qsTr("BibleTime Mobile")
            anchors.left: logo.right
            anchors.verticalCenter: logo.verticalCenter
            anchors.leftMargin: logo.width * 0.2
        }
    }

    Text {
        id: dummyTextForHeight
        text: "x"
        font.pointSize: btStyle.uiFontPointSize
        visible: false
    }

    Rectangle {
        id: outline

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: continueText.top
        anchors.bottom: buttons.bottom
        height: parent.height * 0.25
        anchors.topMargin: btStyle.pixelsPerMillimeterX * -5
        anchors.bottomMargin: btStyle.pixelsPerMillimeterX * -5
        anchors.leftMargin: btStyle.pixelsPerMillimeterX * 5
        anchors.rightMargin: btStyle.pixelsPerMillimeterX * 5
        color: btStyle.textBackgroundColor
        border.color: btStyle.textColor
        border.width: 5
        radius: btStyle.pixelsPerMillimeterX * 3
    }

    Text {
        id: continueText
        width: parent.width
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: btStyle.pixelsPerMillimeterX * 8
        anchors.rightMargin: btStyle.pixelsPerMillimeterX * 8
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        font.pointSize: btStyle.uiFontPointSize
        color: btStyle.textColor
    }

    Grid {
        id: buttons

        spacing: btStyle.pixelsPerMillimeterY * 4
        columns: 2
        anchors.top: continueText.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: btStyle.pixelsPerMillimeterX * 5

        Action {
            id: yesAction
            text: qsTr("Continue")
            onTriggered: {
                continueDialog.visible = false;
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
    }
}
