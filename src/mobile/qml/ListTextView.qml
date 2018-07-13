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

        border.color: btStyle.textColor
        border.width: 1
        color: btStyle.textBackgroundColor
        height: btStyle.uiFontPointSize * 4;
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
            color: btStyle.textColor
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
        ScrollBar.vertical: ScrollBar {
            width: btStyle.pixelsPerMillimeterX * 6
        }


        function selectItem(x, y) {
            var index = listView.indexAt(x+contentX,y+contentY);
            currentIndex = index;
            top.itemSelected(index);
        }

        Rectangle {
            id: background

            color: btStyle.textBackgroundColor
            anchors.fill: parent
            z: -1
        }

        Rectangle {
            id: scrollbar
            anchors.right: listView.right
            y: listView.visibleArea.yPosition * listView.height
            width: 10
            height: listView.visibleArea.heightRatio * listView.height
            color: btStyle.textColor
            visible: listView.visibleArea.heightRatio < 0.99
        }

        delegate: Rectangle {
            id: entry

            property bool selected: ListView.isCurrentItem ? true : false
            objectName: "entry"

            color: (highlight && ListView.isCurrentItem) ? btStyle.textBackgroundHighlightColor : btStyle.textBackgroundColor
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

                anchors.fill: entry
                width: parent.width
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                anchors.topMargin: 10
                verticalAlignment: Text.AlignVCenter
                text: modelText
                color: btStyle.textColor
                font.pointSize: btStyle.uiFontPointSize - 1
                font.bold: highlight && entry.selected
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
