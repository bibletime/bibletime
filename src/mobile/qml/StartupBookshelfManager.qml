import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: startupBookshelfManager

    signal bookshelfRequested()

    color: "white"

    Text {
        width: parent.width
        anchors.fill: parent
        text: "There are currently no works installed. Please click" +
              " on the Ok button below to use the Bookshelf Manager to" +
              " install new works."
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        font.pointSize: btStyle.uiFontPointSize
        anchors.margins: 30
     }

    Grid {
        id: buttons

        spacing: btStyle.pixelsPerMillimeterY * 4
        columns: 2
        anchors.bottom: startupBookshelfManager.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 50

        Rectangle {
            height: btStyle.pixelsPerMillimeterY * 7
            width: btStyle.pixelsPerMillimeterY * 25
            border.color: "black"
            border.width: 4

            Text {
                text: "Ok"
                anchors.centerIn: parent
                font.pointSize: btStyle.uiFontPointSize
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    startupBookshelfManager.visible = false;
                    startupBookshelfManager.bookshelfRequested()
                }
            }
        }

        Rectangle {
            height: btStyle.pixelsPerMillimeterY * 7
            width: btStyle.pixelsPerMillimeterY * 25
            border.color: "black"
            border.width: 4

            Text {
                text: "Cancel"
                anchors.centerIn: parent
                font.pointSize: btStyle.uiFontPointSize
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    startupBookshelfManager.visible = false;
                }
            }
        }
    }

    BtStyle {
        id: btStyle
    }
}
