import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: menuButton

    signal buttonClicked

    color: "black"

    BtStyle {
        id: btStyle
    }

    Column {
        spacing:8

        Rectangle { color: "white"; width:5; height:5 }
        Rectangle { color: "white"; width:5; height:5 }
        Rectangle { color: "white"; width:5; height:5 }

        anchors.centerIn: parent
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            menuButton.buttonClicked()
        }
    }
}
