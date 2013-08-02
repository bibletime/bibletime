import QtQuick 2.1
import BibleTime 1.0



MouseArea {
    id: menus

    property alias model: menusRepeater.model

    signal menuSelected(string action)

    z:50
    visible: false
    anchors.fill: parent
    enabled: menus.visible
    onClicked: {
        menus.visible = false;
    }

    Rectangle {

        width:190
        height: btStyle.menuHeight * model.count+6
        color: btStyle.menu
        border.color: "black"
        border.width: 2
        z: 100
        anchors.right: parent.right
        anchors.top: parent.top

        Column {
            id: menuColumn

            width:parent.width-4
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.topMargin: 3

            Repeater {
                id: menusRepeater

                delegate: Rectangle {

                    width:parent.width-4
                    height:btStyle.menuHeight
                    color: btStyle.menu
                    border.color: btStyle.menuBorder
                    border.width: 1

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 6
                        text: title
                        font.pointSize: 12
                        color: btStyle.menuText
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            menus.menuSelected(action);
                        }
                    }
                }
            }
        }
    }
}




