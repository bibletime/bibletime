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
import QtQuick.Controls.Styles 1.2
import BibleTime 1.0

Rectangle {
    id: installProgress

    property alias minimumValue: progressBar.minimumValue
    property alias value: progressBar.value
    property alias maximumValue: progressBar.maximumValue
    property alias text: label.text

    color: btStyle.textBackgroundColor
    border.color: btStyle.textColor
    border.width: 5
    radius: btStyle.pixelsPerMillimeterX * 3

    signal cancel()

    BtStyle {
        id: btStyle
    }

    Text {
        id: label

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: progressBar.top
        anchors.bottomMargin: parent.height / 8
        font.pointSize: btStyle.uiFontPointSize
        color: btStyle.textColor
    }

    ProgressBar {
        id: progressBar

        anchors.centerIn: parent
        width: parent.width - 100
        height: parent.height /10
        style: ProgressBarStyle {
            background: Rectangle {
                radius: 2
                color: "lightgray"
                border.color: "gray"
                border.width: 1
                implicitWidth: 200
                implicitHeight: 24
            }
        }
    }

    Action {
        id: cancelAction
        text: qsTranslate("Progress", "Cancel")
        onTriggered: {
            cancel();
        }
    }

    Button {
        id: cancelButton
        height: btStyle.pixelsPerMillimeterY * 7
        width: btStyle.pixelsPerMillimeterY * 25
        anchors.top: progressBar.bottom
        anchors.topMargin: parent.height / 8
        anchors.horizontalCenter: parent.horizontalCenter
        action: cancelAction
        style: BtButtonStyle {
        }
    }
}
