import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: quitQuestion

    anchors.fill: parent
    color: "white"
    visible: false
    width: parent.width

    Text {
        id: text
        height: contentHeight
        width: parent.width
        anchors.centerIn: parent
        text: "Are you sure you want to quit?"
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
        anchors.top: text.bottom
        anchors.topMargin: 50
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 50

        Rectangle {
            height: btStyle.pixelsPerMillimeterY * 7
            width: btStyle.pixelsPerMillimeterY * 25
            border.color: "black"
            border.width: 4

            Text {
                text: "Yes"
                anchors.centerIn: parent
                font.pointSize: btStyle.uiFontPointSize
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    quitQuestion.visible = false;
                    Qt.quit();
                }
            }
        }

        Rectangle {
            height: btStyle.pixelsPerMillimeterY * 7
            width: btStyle.pixelsPerMillimeterY * 25
            border.color: "black"
            border.width: 4

            Text {
                text: "No"
                anchors.centerIn: parent
                font.pointSize: btStyle.uiFontPointSize
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    quitQuestion.visible = false;
                }
            }
        }
    }

}
