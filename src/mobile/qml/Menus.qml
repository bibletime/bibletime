import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: menu

    property alias model: menusRepeater.model
    property int fontPointSize: 15
    property int menuHeight: {
        var pixel = btStyle.pixelsPerMillimeterY * 9;
        var uiFont = btStyle.uiFontPointSize * 3.5;
        return Math.max(pixel, uiFont);
    }
    property int topMenuMargin: 150
    property int leftMenuMargin: 50

    signal menuSelected(string action)

    Keys.onReleased: {
        if (event.key == Qt.Key_Back) {
            event.accepted = true;
            menu.visible = false;
        }
    }

    visible: false
    anchors.fill: parent
    color: "#f0f0f0"

    BtStyle {
        id: btStyle
    }

    Component {
        id: eachMenu

        Rectangle {
            width: menu.width
            height: menuHeight
            color: "white"
            border.color: "#f0f0f0"
            border.width: 2

            Text {
                text: title
                font.pointSize: btStyle.uiFontPointSize
                color: "black"
                anchors.fill: parent
                anchors.leftMargin: leftMenuMargin
                verticalAlignment: Text.AlignVCenter

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        menu.menuSelected(action);
                    }
                }
            }
        }
    }

    ListView {
        id: menusRepeater

        delegate: eachMenu
        width: parent.width
        anchors.fill: parent
        anchors.topMargin: topMenuMargin

    }
}

