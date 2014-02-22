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

Rectangle {
    id: top

    property alias model: listView.model
    property alias currentIndex: listView.currentIndex
    property alias title: title.text
    property bool highlight: true

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
            horizontalAlignment: Text.AlignCenter
            verticalAlignment: Text.AlignBottom
            style: Text.Sunken
            font.pointSize: btStyle.uiFontPointSize
        }
    }

    ListView {
        id: listView

        anchors.top: titleRect.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 3
        anchors.rightMargin: 3
        anchors.bottomMargin: 3
        clip: true
        highlightFollowsCurrentItem: true
        currentIndex: 2

        function selectItem(x, y) {
            var index = listView.indexAt(x+contentX,y+contentY);
            currentIndex = index;
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

        delegate {
            Rectangle {
                id: entry

                property bool selected: ListView.isCurrentItem ? true : false
                objectName: "entry"

                color: (highlight && ListView.isCurrentItem) ? "#ffeeaa" : "white"
                border.width: buttonMouseArea.pressed ? 5 :1
                border.color: "darkgray"
                width: parent.width
                height: {
                    var pixel = btStyle.pixelsPerMillimeterY * 8;
                    var uiFont = btStyle.uiFontPointSize * 3.5;
                    return Math.max(pixel, uiFont);
                }

                Text {
                    id: entryText

                    anchors.top: entry.top
                    anchors.left: entry.left
                    anchors.right: entry.right
                    width: parent.width
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    anchors.topMargin: 10
                    verticalAlignment: Text.AlignVCenter
                    text: modelText
                    font.pointSize: btStyle.uiFontPointSize
                    font.bold: highlight && entry.selected
                }
            }
        }

        MouseArea {
            id: buttonMouseArea

            anchors.fill: listView
            onPressed: {

            }

            onClicked: itemSelected()

            function itemSelected() {
                listView.selectItem(mouseX, mouseY);
            }
        }
    }
}
