import QtQuick 2.0

ListView {
    id: listView

    clip: true
    highlightFollowsCurrentItem: true
    currentIndex: 2

    signal itemSelected(int index)

    function selectItem(x, y) {
        var index = listView.indexAt(x+contentX,y+contentY);
        currentIndex = index;
        itemSelected(index);
    }

    Rectangle {
        id: scrollbar
        anchors.right: listView.right
        y: listView.visibleArea.yPosition * listView.height
        width: 6
        height: listView.visibleArea.heightRatio * listView.height
        color: "darkgray"
    }

    delegate {
        Rectangle {
            id: entry

            property bool selected: ListView.isCurrentItem ? true : false
            objectName: "entry"
            color: ListView.isCurrentItem ? "#ffeeaa" : "white"
            border.width: 1
            border.color: "darkgray"
            width: parent.width
            height: 40

            Text {
                id: entryText

                anchors.top: entry.top
                anchors.left: entry.left
                anchors.right: entry.right
                width: parent.width
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                anchors.topMargin: 10
                text: modelText
                font.bold: entry.selected
            }
        }
    }

    MouseArea {
        anchors.fill: listView
        onClicked: itemSelected()

        function itemSelected() {
            listView.selectItem(mouseX, mouseY);
        }
    }
}
