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
import QtQuick.Controls 1.2

Rectangle {
    id: fontPointSize

    property string title: ""
    property int min: 10
    property int max: 30
    property int current: 20
    property int previous: 20

    signal accepted(int pointSize);

    color: "#f8f8f8"
    border.color: "black"
    border.width: 1
    anchors.centerIn: parent
    width: parent.width * 0.95
    height: btStyle.pixelsPerMillimeterY * 18 + 50

    Text {
        text: qsTranslate("main", title)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 10
        height: btStyle.pixelsPerMillimeterY * 7
        font.pointSize: btStyle.uiFontPointSize
    }

    Rectangle {
        id: bar

        color: "blue"
        width: parent.width *.90
        height: 3
        anchors.centerIn: parent
    }

    Rectangle {
        id: indicator

        width: btStyle.pixelsPerMillimeterX * 3
        height: width
        color: "red"
        y: bar.y - height / 2
        x: {
            var range = fontPointSize.max - fontPointSize.min;
            var xpos = bar.width *
                    (fontPointSize.current - fontPointSize.min) / range;
            xpos = xpos + bar.x
            return xpos
        }
    }

    MouseArea {
        property bool active: false
        width: bar.width
        anchors.left: bar.left
        height: btStyle.pixelsPerMillimeterY * 7
        anchors.verticalCenter: bar.verticalCenter

        onPressed: {
            active = true;
        }

        onReleased: {
            active = false;
        }

        onMouseXChanged: {
            if ( ! active)
                return;
            var range = fontPointSize.max - fontPointSize.min;
            var currentF = mouse.x / bar.width * range  + fontPointSize.min;
            var value = Math.round(currentF);
            if (value < fontPointSize.min)
                value = min;
            if (value > fontPointSize.max)
                value = max;
            fontPointSize.current = value;
            accepted(value);
        }

    }

    Grid {
        id: buttons

        spacing: btStyle.pixelsPerMillimeterY * 5
        columns: 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 15
        anchors.topMargin: 45

        Action {
            id: okAction
            text: qsTr("Ok")
            onTriggered: {
                fontPointSize.visible = false;
            }
        }

        Button {
            id: okButton
            height: btStyle.pixelsPerMillimeterY * 6
            width: btStyle.pixelsPerMillimeterY * 18
            action: okAction
            style: BtButtonStyle {
            }
        }

        Action {
            id: cancelAction
            text: qsTr("Cancel")
            onTriggered: {
                accepted(previous);
                fontPointSize.visible = false;
            }
        }

        Button {
            id: cancelButton
            height: btStyle.pixelsPerMillimeterY * 6
            width: btStyle.pixelsPerMillimeterY * 18
            action: cancelAction
            style: BtButtonStyle {
            }
        }
    }
}
