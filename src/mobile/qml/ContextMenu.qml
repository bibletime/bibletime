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
    id: contextMenu

    property alias model: menuList.model

    signal accepted(string action)

    color: "white"
    border.color: "black"
    border.width: 1
    anchors.centerIn: parent
    height: 250
    width: 250

    onVisibleChanged: PropertyAnimation {
        target: settings
        property: "opacity"
        from: 0
        to: 1
        duration: 400
        easing.type: Easing.InOutCubic
    }

    BtStyle {
        id: btStyle
    }

    ListView {
        id: menuList

        anchors.fill: parent
        anchors.topMargin: 100

        delegate: Rectangle {
            color: "white"
            border.color: "black"
            border.width: 1
            width: parent.width
            height: children[0].height * 2.5

            Text {
                id: menuText
                x: 40
                anchors.verticalCenter: parent.verticalCenter
                text: title
                color: "black"
                font.pointSize: btStyle.uiFontPointSize
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    contextMenu.visible = false;
                    contextMenu.accepted(action);
                }
            }
        }
    }
}
