import QtQuick 2.1
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

        delegate {
            Rectangle {
                id: entry

                color: "white"
                border.width: 1
                border.color: ListView.isCurrentItem ? "#c0c0c0" : "#a0a0a0"
                width: parent.width
                height: {
                    var pixel = btStyle.pixelsPerMillimeterY * 7;
                    var uiFont = btStyle.uiFontPointSize * 5;
                    return Math.max(pixel, uiFont);
                }

                Image {
                    id: installedCheckmark

                    source: "checkmark.svg"
                    height: entry.height - 15
                    width:  entry.height -15
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin: 5
                    anchors.topMargin: 5
                    visible: installed == 1
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {listView.itemSelected(index)}
                }

                Text {
                    anchors.top: entry.top
                    anchors.left: installedCheckmark.right
                    anchors.right: entry.right
                    width: parent.width
                    height: parent.height/2 -4
                    anchors.leftMargin: 5
                    anchors.rightMargin: 10
                    anchors.topMargin: 5
                    verticalAlignment: Text.AlignVCenter
                    text: title
                    font.pointSize: btStyle.uiFontPointSize
                }

                Text {
                    anchors.bottom: entry.bottom
                    anchors.left: installedCheckmark.right
                    anchors.right: entry.right
                    width: parent.width
                    height: parent.height/2 -4
                    anchors.leftMargin: 15
                    anchors.rightMargin: 10
                    anchors.bottomMargin: 5
                    text: desc
                    elide: Text.ElideMiddle
                    font.pointSize: btStyle.uiFontPointSize- 2
                }
            }
        }

        BtStyle {
            id: btStyle
        }
    }
}

