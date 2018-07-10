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
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.3

Rectangle {
    id: fontPointSize

    property string title: ""
    property int min: 10
    property int max: 30
    property int current: 20
    property int previous: 20
    property bool ready: false

    signal accepted(int pointSize);

    color: btStyle.textBackgroundColor
    border.color: btStyle.textColor
    border.width: 3
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.topMargin: 20
    anchors.bottomMargin: btStyle.pixelsPerMillimeterX * 2
    anchors.rightMargin: btStyle.pixelsPerMillimeterX * 2
    height: {
        var height = titleText.contentHeight + slider.height * 1.4 + buttons.height;
        height = height + titleText.anchors.topMargin;
        height = height + buttons.anchors.bottomMargin + buttons.anchors.topMargin;
        height = height + btStyle.pixelsPerMillimeterX*3
        return height;
    }
    width: {
        var width = Math.min(parent.width, parent.height);
        width = width - 2 * anchors.rightMargin
        return width;
    }
    onVisibleChanged: {
        fontPointSize.ready = false;
        if (visible) {
            var uiTextSize = btStyle.uiFontPointSize;
            slider.value = uiTextSize;
            fontPointSize.ready = true;
        }
    }
    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && fontPointSize.visible == true) {
            accepted(previous);
            fontPointSize.visible = false;
            event.accepted = true;
        }
    }

    Text {
        id: titleText

        text: qsTranslate("main", title)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 20
        font.pointSize: btStyle.uiFontPointSize
        color: btStyle.textColor
    }

    Slider {
        id: slider

        anchors.top: titleText.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 60
        anchors.topMargin: 20
        minimumValue: fontPointSize.min
        maximumValue: fontPointSize.max
        style: SliderStyle {
                groove: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 8
                    color: btStyle.textColor
                    radius: 8
                }
                handle: Rectangle {
                    anchors.centerIn: parent
                    color: btStyle.textBackgroundColor
                    border.color: btStyle.textColor
                    border.width: 3
                    implicitWidth: btStyle.pixelsPerMillimeterY * 7
                    implicitHeight: btStyle.pixelsPerMillimeterY * 7
                    radius: btStyle.pixelsPerMillimeterY * 3.5
                }
            }
        onValueChanged: {
            if (fontPointSize.ready)
                 accepted(slider.value);
        }
    }

    Grid {
        id: buttons

        spacing: btStyle.pixelsPerMillimeterY * 4
        columns: 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        anchors.topMargin: 30

        Action {
            id: okAction
            text: qsTr("Ok")
            onTriggered: {
                fontPointSize.visible = false;
            }
        }

        Button {
            id: okButton
            height: titleText.height*1.5
            width: fontPointSize.width/3.5
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
            height: titleText.height*1.5
            width: fontPointSize.width/3.5
            action: cancelAction
            style: BtButtonStyle {
            }
        }
    }
}
