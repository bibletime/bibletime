import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.0
import BibleTime 1.0

Rectangle {
    id: installProgress

    property alias minimumValue: progressBar.minimumValue
    property alias value: progressBar.value
    property alias maximumValue: progressBar.maximumValue
    property alias text: label.text

    color: btStyle.buttonBackground
    border.color: "black"
    border.width: 5

    signal cancel()

    BtStyle {
        id: btStyle
    }

    Text {
        id: label

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: progressBar.top
        anchors.bottomMargin: 20
        font.pointSize: btStyle.uiFontPointSize
    }

    ProgressBar {
        id: progressBar

        anchors.centerIn: parent
        width: parent.width - 100
        height: 16
    }

    Rectangle {
        color: "white"
        border.color: "black"
        border.width: 1
        width: btStyle.pixelsPerMillimeterX * 25
        height: btStyle.pixelsPerMillimeterY * 7
        anchors.top: progressBar.bottom
        anchors.topMargin: 20
        anchors.horizontalCenter: parent.horizontalCenter

        Text {
            anchors.centerIn: parent
            text: "Cancel"
            font.pointSize: btStyle.uiFontPointSize

        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                cancel();
            }
        }
    }
}
