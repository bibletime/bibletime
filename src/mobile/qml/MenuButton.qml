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
        spacing:height/5

        Rectangle { color: "black"; width:menuButton.height/10; height:menuButton.height/10 }
        Rectangle { color: "black"; width:menuButton.height/10; height:menuButton.height/10 }
        Rectangle { color: "black"; width:menuButton.height/10; height:menuButton.height/10 }

        anchors.centerIn: parent
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            menuButton.buttonClicked()
        }
    }
}
