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
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import BibleTime 1.0

Rectangle {
    id: top
    property alias model: listView.model
    property alias title: title.text

    border.color: "black"
    border.width: 2

    signal itemSelected(int index)

    Rectangle {
        id: titleRect

        border.color: "black"
        border.width: 1
        height: btStyle.uiFontPointSize * 3;
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 3
        anchors.rightMargin: 3
        anchors.topMargin: 3

        Text {
            id: title
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.centerIn: parent
            anchors.topMargin: 5
            horizontalAlignment: Text.AlignCenter
            verticalAlignment: Text.AlignBottom
            style: Text.Sunken
            font.pointSize: btStyle.uiFontPointSize
        }
    }

    ListView {
        id: listView
        clip: true
        anchors.top: titleRect.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 3

        function itemSelected(index) {
            top.itemSelected(index);
        }

        Rectangle {
            id: scrollbar
            anchors.right: listView.right
            y: listView.visibleArea.yPosition * listView.height
            width: 7
            height: listView.visibleArea.heightRatio * listView.height
            color: "black"
            visible: listView.visibleArea.heightRatio < 0.99
        }

        delegate: Rectangle {
            id: entry

            color: "white"
            border.width: 1
            border.color: ListView.isCurrentItem ? "#c0c0c0" : "#a0a0a0"
            width: parent.width
            height: {
                var pixel = btStyle.pixelsPerMillimeterY * 7;
                var uiFont1 = titleText.contentHeight;
                var uiFont2 = descriptionText.contentHeight;
                var uiFont = Math.max(uiFont1, uiFont2);
                var uiHeight = Math.max(pixel, uiFont);
                return uiHeight * 1.05;
            }

            Action {
                id: manageAction
                text: {
                 if (installed)
                    return QT_TR_NOOP("Remove");
                 else
                     return QT_TR_NOOP("Install");
                }
                onTriggered: {
                    listView.itemSelected(index);
                    manageButton.checked = ! manageButton.checked;
                    manageButton.chooseGradient();
                }
            }

            Gradient {
                id: gradient1
                GradientStop { position: 0 ; color: manageButton.pressed ? "#ccc" : "#eee" }
                GradientStop { position: 1 ; color: manageButton.pressed ? "#aaa" : "#ccc" }
            }

            Gradient {
                id: gradient2
                GradientStop { position: 0 ; color: manageButton.pressed ? "#dbb" : "#fdd" }
                GradientStop { position: 1 ; color: manageButton.pressed ? "#b99" : "#dbb" }
            }

            Gradient {
                id: gradient3
                GradientStop { position: 0 ; color: manageButton.pressed ? "#bdb" : "#dfd" }
                GradientStop { position: 1 ; color: manageButton.pressed ? "#9b9" : "#bdb" }
            }

            Button{
                id: manageButton

                property Gradient buttonGradient : gradient1

                function chooseGradient() {
                    if (manageButton.checked) {
                        if (installed)
                            buttonGradient = gradient2;
                        else
                            buttonGradient = gradient3;
                    }
                    else {
                        buttonGradient = gradient1;
                    }
                }

                action: manageAction
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 10
                width: btStyle.uiFontPointSize * 13
                height: btStyle.uiFontPointSize * 6

                checkable: true;
                style: ButtonStyle {
                    id: buttonStyle

                    background: Rectangle {
                        implicitWidth: 100
                        implicitHeight: 25
                        border.width: manageButton.checked ? 7 : 1
                        border.color: manageButton.checked ? "black" : "#777"
                        radius: 4

                        gradient: manageButton.buttonGradient
                    }
                    label: Text {
                        anchors.centerIn: parent
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: btStyle.uiFontPointSize -1
                        color: "black"
                        text: control.text
                    }
                }
            }

            Text {
                id: titleText

                anchors.verticalCenter: entry.verticalCenter
                anchors.left: manageButton.right
                width: btStyle.uiFontPointSize * 16
                height: parent.height/3 -4
                anchors.leftMargin: btStyle.pixelsPerMillimeterX
                anchors.rightMargin: 10
                anchors.topMargin: 5
                verticalAlignment: Text.AlignVCenter
                text: title
                font.pointSize: btStyle.uiFontPointSize - 1
            }

            Text {
                id: descriptionText

                anchors.left: titleText.right
                anchors.right: entry.right
                anchors.verticalCenter: entry.verticalCenter
                width: parent.width
                wrapMode: Text.WordWrap
                anchors.leftMargin: btStyle.pixelsPerMillimeterX
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                text: desc
                elide: Text.ElideMiddle
                font.pointSize: btStyle.uiFontPointSize- 2
            }
        }

        BtStyle {
            id: btStyle
        }
    }
}

