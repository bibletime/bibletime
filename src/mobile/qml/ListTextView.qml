import QtQuick 2.0

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
        height:25
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
                    font.bold: highlight && entry.selected
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
}
