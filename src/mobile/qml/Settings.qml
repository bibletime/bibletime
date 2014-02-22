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
    id: settings

    property int finalHeight: 300

    color: "white"
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    height: parent.height

    onVisibleChanged: PropertyAnimation {
        target: settings
        property: "opacity"
        from: 0
        to: 1
        duration: 200
        easing.type: Easing.InOutCubic
    }

    BtStyle {
        id: btStyle
    }

    ListModel {
        id: settingsModel

        ListElement { title: "Ui Font Size";   action: "uiSize" }
        ListElement { title: "Window Arrangement";   action: "arrangement" }
    }

    ListView {
        id: settingsList

        anchors.fill: parent
        anchors.topMargin: 100
        model: settingsModel

        delegate: Rectangle {
            color: "white"
            border.color: "lightgray"
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
                    if (action == "arrangement") {
                        windowArrangementMenus.visible = true;
                        settings.visible = false;
                    }
                    else if (action == "uiSize") {
                        uiFontPointSize.visible = true;
                        settings.visible = false;
                        console.log("y")
                    }
                }
            }
        }
    }

    ImageButton {
        id: backButton

        icon: "leftarrow.svg"
        height: 36
        width:  56
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 8
        visible: true

        MouseArea {
            anchors.fill: parent
            onClicked: {
                settings.visible = false;
            }
        }
    }
}
