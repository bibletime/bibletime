import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: startupBookshelfManager

    signal bookshelfRequested()

    height:btStyle.pixelsPerMillimeterY * 48

    Text {
        anchors.fill: parent
        text: "There are currently no works installed. Please click" +
              " on the Ok button below to use the Bookshelf Manager to" +
              " install new works."
        verticalAlignment: Text.verticalAlignment
        horizontalAlignment: Text.horizontalAlignment
        font.pointSize: btStyle.uiFontPointSize
        anchors.margins: 30
        wrapMode: Text.WordWrap
    }

    Grid {
        id: buttons

        spacing: btStyle.pixelsPerMillimeterY * 4
        columns: 2
        anchors.bottom: startupBookshelfManager.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 10

        Rectangle {
            height: btStyle.pixelsPerMillimeterY * 7
            width: btStyle.pixelsPerMillimeterY * 25
            border.color: "black"
            border.width: 1

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
            border.width: 1

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
