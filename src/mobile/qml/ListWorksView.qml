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
import QtQuick.Controls 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
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
        height: btStyle.uiFontPointSize * 4;
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.leftMargin: 3
        anchors.rightMargin: 3
        anchors.topMargin: 3
        color: btStyle.textBackgroundColor

        Text {
            id: title
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.centerIn: parent
            anchors.topMargin: 5
            horizontalAlignment: Text.AlignCenter
            verticalAlignment: Text.AlignBottom
            style: Text.Sunken
            font.pointSize: btStyle.uiFontPointSize
            color: btStyle.textColor
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
        ScrollBar.vertical: ScrollBar {
            width: btStyle.pixelsPerMillimeterX * 6
        }

        function itemSelected(index) {
            top.itemSelected(index);
        }

        Rectangle {
            id: background

            color: btStyle.textBackgroundColor
            anchors.fill: parent
            z: -1
        }

        delegate: Rectangle {
            id: entry

            color: btStyle.textBackgroundColor
            border.width: 1
            border.color: ListView.isCurrentItem ? "#c0c0c0" : "#a0a0a0"
            width: parent.width
            height: {
                var pixel = btStyle.pixelsPerMillimeterY * 7;
                var uiFont = titleText.contentHeight;
                var uiHeight = Math.max(pixel, uiFont);
                return uiHeight * 1.25;
            }

            Text {
                id: dummyTextForHeight
                text: qsTranslate("Install Documents", "Install")
                font.pointSize: btStyle.uiFontPointSize
                visible: false
            }

            Action {
                id: manageAction
                text: {
                 if (installed)
                    return qsTranslate("Install Documents", "Remove");
                 else
                     return qsTranslate("Install Documents", "Install");
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
                GradientStop { position: 0 ; color: manageButton.pressed ? "#daa" : "#fcc" }
                GradientStop { position: 1 ; color: manageButton.pressed ? "#b88" : "#daa" }
            }

            Gradient {
                id: gradient3
                GradientStop { position: 0 ; color: manageButton.pressed ? "#ada" : "#cfc" }
                GradientStop { position: 1 ; color: manageButton.pressed ? "#8b8" : "#ada" }
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
                width: dummyTextForHeight.width*1.1
                height: dummyTextForHeight.height*1.5

                checkable: true;
                style: ButtonStyle {
                    id: buttonStyle

                    background: Rectangle {
                        implicitWidth: 100
                        implicitHeight: 25
                        border.width: manageButton.checked ? 4 : 1
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
                anchors.right: entry.right
                height: entry.height
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                anchors.leftMargin: btStyle.pixelsPerMillimeterX
                anchors.rightMargin: 10
                anchors.topMargin: 5
                verticalAlignment: Text.AlignVCenter
                text: "<b>" + title + "</b> - " + desc
                font.pointSize: btStyle.uiFontPointSize
                color: btStyle.textColor
            }
        }

        BtStyle {
            id: btStyle
        }
    }
}

