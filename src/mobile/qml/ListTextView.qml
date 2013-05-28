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

    delegate {
        Rectangle {
            id: entry

            property bool selected: ListView.isCurrentItem ? true : false
            objectName: "entry"
            color: ListView.isCurrentItem ? "#ffffdd" : "white"
            border.width: selected ? 3 : 1
            border.color: ListView.isCurrentItem ? "black" : "#a0a0a0"
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
