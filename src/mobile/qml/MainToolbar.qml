import QtQuick 2.0
import BibleTime 1.0

Rectangle {
    id: toolbar

    signal buttonClicked

    BtStyle {
        id: btStyle
    }

    color: btStyle.toolbarColor
    z:0

    MenuButton {
        id: menuButton

        width: parent.height
        height: parent.height
        anchors.right: parent.right
        anchors.top: parent.top
        onButtonClicked: {
            toolbar.buttonClicked()
        }
    }
}
