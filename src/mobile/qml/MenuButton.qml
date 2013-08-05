import QtQuick 2.0
import BibleTime 1.0

Rectangle {
    id: menuButton

    signal buttonClicked

    color: "black"

    BtStyle {
        id: btStyle
    }

    Column {
        spacing:3

        Rectangle { color: "white"; width:2; height:2 }
        Rectangle { color: "white"; width:2; height:2 }
        Rectangle { color: "white"; width:2; height:2 }

        anchors.centerIn: parent
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            menuButton.buttonClicked()
        }
    }
}
