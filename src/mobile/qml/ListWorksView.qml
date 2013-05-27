import QtQuick 2.0
import BibleTime 1.0

ListView {
    clip: true

    delegate {
        Rectangle {
            id: entry

            color: "white"
            border.width: 1
            border.color: ListView.isCurrentItem ? "#c0c0c0" : "#a0a0a0"
            width: parent.width
            height: 44

            Rectangle {
                id: button

                height: parent.height -8
                width: 80
                color: "lightgray"
                anchors.right: entry.right
                anchors.top: entry.top
                anchors.topMargin: 4
                anchors.rightMargin: 6

                gradient: Gradient {
                    GradientStop { position: 0.0;  color: btStyle.buttonGradient0 }
                    GradientStop { position: 0.15; color: btStyle.buttonGradient1 }
                    GradientStop { position: 0.85; color: btStyle.buttonGradient2 }
                    GradientStop { position: 1.0;  color: btStyle.buttonGradient3 }
                }
                smooth: true

                border {
                    width: 1
                    color: btStyle.buttonBorder
                }

                Text {

                    text: "Install"
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        console.log("Install " + title)
                    }
                }

            }
            Text {
                anchors.top: entry.top
                anchors.left: entry.left
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
                anchors.left: entry.left
                anchors.right: entry.right
                width: parent.width
                height: parent.height/2 -4
                anchors.leftMargin: 35
                anchors.rightMargin: button.width + 10
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

