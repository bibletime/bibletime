import QtQuick 2.0
import BibleTime 1.0

ListView {
    id: listView
    clip: true

    signal itemSelected(int index)

    delegate {
        Rectangle {
            id: entry

            color: "white"
            border.width: 1
            border.color: ListView.isCurrentItem ? "#c0c0c0" : "#a0a0a0"
            width: parent.width
            height: 44

            Image {
                id: installedCheckmark

                source: "checkmark.svg"
                height: entry.height - 15
                width:  25
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin: 5
                anchors.topMargin: 5
                visible: installed == 1
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    onClicked: listView.itemSelected(index);
                }

            }
            Text {
                anchors.top: entry.top
                anchors.left: installedCheckmark.right
                anchors.right: entry.right
                width: parent.width
                height: parent.height/2 -4
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                anchors.topMargin: 5
                text: title
                font.pointSize: 11
            }

            Text {
                anchors.bottom: entry.bottom
                anchors.left: installedCheckmark.right
                anchors.right: entry.right
                width: parent.width
                height: parent.height/2 -4
                anchors.leftMargin: 35
                anchors.rightMargin: 10
                anchors.topMargin: 25
                text: desc
                elide: Text.ElideMiddle
                font.pointSize: 11
            }
        }
    }

    BtStyle {
        id: btStyle
    }
}

