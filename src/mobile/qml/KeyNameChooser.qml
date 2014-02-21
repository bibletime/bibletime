import QtQuick 2.1
import QtQuick.Controls 1.1
import BibleTime 1.0

Rectangle {
    id: top

    property alias model: listView.model
    property alias currentIndex: listView.currentIndex
    property bool highlight: true

    border.color: "black"
    border.width: 2

    signal itemSelected(int index)

    Keys.onReleased: {
        if (event.key == Qt.Key_Back) {
            event.accepted = true;
            top.visible = false;
        }
    }

    BtStyle {
        id: btStyle
    }

    SearchModel {
        id: searchModel
    }

    Rectangle {
        id: searchRect

        color: "#a0d0ff"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: btStyle.pixelsPerMillimeterY * 8
        border.color: "gray"
        border.width: 1

        Row {
            id: searchRow

            anchors.fill: parent
            anchors.leftMargin: 20

            Text {
                text: "Search "
                height: btStyle.pixelsPerMillimeterY * 6
                verticalAlignment: TextEdit.AlignVCenter
                font.pointSize: btStyle.uiFontPointSize
                color: "black"
            }

            Rectangle {

                height: searchRect.height - 5
                anchors.verticalCenter: parent.verticalCenter
                width: height * 10
                color: "white"
                border.color: "gray"
                border.width: 1

                TextField {
                    id: textEdit

                    text: ""
                    height: parent.height
                    width: height * 10
                    anchors.top: parent.top
                    anchors.left: parent.left
                    inputMethodHints: Qt.ImhNoPredictiveText
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: btStyle.uiFontPointSize
                    focus: true
                    onTextChanged: {
                        var matchingIndex = searchModel.indexMatchingKeyName(text, listView.model);
                        listView.positionViewAtIndex(matchingIndex, ListView.Beginning);
                    }
                }
            }
        }
    }

    ListView {
        id: listView

        anchors.top: searchRect.bottom
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
                    text: keyName
                    font.pointSize: btStyle.uiFontPointSize
                    font.bold: highlight && entry.selected
                }
            }
        }

        MouseArea {
            anchors.fill: listView
            onClicked: itemSelected()

            function itemSelected() {
                listView.selectItem(mouseX, mouseY);
                console.log(listView.currentIndex);
                top.visible = false;
            }
        }
    }
}
