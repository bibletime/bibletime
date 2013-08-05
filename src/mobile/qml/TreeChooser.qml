import QtQuick 2.0

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

    Rectangle {
        id: pathArea

        border.color: "black"
        border.width: 0
        color: "white"

        height: {20 * pathCount }
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
            height: 36
            width:  56
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
            font.pointSize: 12
            height: parent.height
//            width: parent.width - backButton.width -50
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
            height: 40

            Text {
                id: entryText

                font.pointSize: 12
//                anchors.fill: entry
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

                height: parent.height-4
                width:  56
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
