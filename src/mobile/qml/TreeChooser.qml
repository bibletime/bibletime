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
    id: treeChooser

    property ListModel model: ListModel {
    }
    property string path: ""
    property int pathCount: 0

    color: "white"
    border.width: 2
    border.color: "black"

    signal back()
    signal next(string childText)
    signal select(string childText)

    Keys.onReleased: {
        if (event.key == Qt.Key_Back) {
            event.accepted = true;
            treeChooser.visible = false;
        }
    }

    Rectangle {
        id: pathArea

        border.color: "black"
        border.width: 0
        color: "white"

        height: btStyle.uiFontPointSize * (5 + pathCount *3);
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 4
        visible: path.length > 0

        function splitPath(path) {
            var pathList = path.split("/");
            var newPath = "";
            var space = "";
            var count = pathList.length;
            treeChooser.pathCount = count;
            // pathList[0] is empty
            for (var i=1; i< count; ++i) {
                var pathI = pathList[i];
                newPath += space + pathI + "\n";
                space += "    ";
            }
            return newPath;
        }

        ImageButton {
            id: backButton

            icon: "leftarrow.svg"
            height: btStyle.uiFontPointSize * 4;
            width:  btStyle.uiFontPointSize * 4;
            anchors.verticalCenter: parent.verticalCenter
            anchors.right : parent.right
            anchors.topMargin: 2
            visible: true

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    treeChooser.back();
                }
            }
        }

        Text {
            id: pathText

            text: pathArea.splitPath(treeChooser.path)
            height: btStyle.uiFontPointSize * 5;
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: backButton.left
            anchors.leftMargin: 10
            elide: Text.ElideRight
        }
    }

    ListView {
        id: listView

        function next(index, name) {
            treeChooser.next(name);
        }

        function select(index, name) {
            treeChooser.select(name);
        }

        anchors.top: pathArea.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 30
        anchors.leftMargin: 4
        anchors.rightMargin: 4
        anchors.bottomMargin: 4
        boundsBehavior: Flickable.StopAtBounds
        width: pathArea.width
        model: treeChooser.model

        delegate:
            Rectangle {
            id: entry

            property string action: ""


            border.color: "#eeeeee"
            border.width: 1
            width: parent.width
            height: btStyle.uiFontPointSize * 5;

            Text {
                id: entryText

                font.pointSize: btStyle.uiFontPointSize
                anchors.top: entry.top
                anchors.left: entry.left
                anchors.right: entry.right
                width: 340
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                anchors.topMargin: 10
                text: name
                elide: Text.ElideRight
            }

            ImageButton {
                id: imageButton
                icon: "rightarrow.svg"

                height: btStyle.uiFontPointSize * 4;
                width:  btStyle.uiFontPointSize * 4;
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.topMargin: 2
                visible: childcount > 0

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        listView.currentIndex = index
                        listView.next(index, name)
                    }
                }
            }

            MouseArea {
                anchors.left: parent.left
                anchors.right: imageButton.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                onClicked: {
                    listView.currentIndex = index
                    listView.select(index, name)
                }
            }
        }

        snapMode: ListView.SnapToItem
        focus: true
    }
}
