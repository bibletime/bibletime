import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: menuButton

    signal buttonClicked

    color: "#a0d0ff"

    BtStyle {
        id: btStyle
    }

    Column {
        id: column
        property real ppmy: btStyle.pixelsPerMillimeterY

        spacing: column.ppmy * 0.3

        Rectangle {
            color: "black";
            width: column.ppmy * 0.7
            height:column.ppmy * 0.7
        }
        Rectangle {
            color: "black";
            width:column.ppmy * 0.7
            height:column.ppmy * 0.7
        }
        Rectangle {
            color: "black";
            width:column.ppmy * 0.7
            height:column.ppmy * 0.7
        }

        anchors.centerIn: parent
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            menuButton.buttonClicked()
        }
    }
}
