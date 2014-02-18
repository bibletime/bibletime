import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: toolbar

    color: btStyle.toolbarColor
    z:0

    signal buttonClicked

    BtStyle {
        id: btStyle
    }

    Image {
        id: logo

        width: parent.height - 10
        height: parent.height - 10
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.bottomMargin: 5
        anchors.leftMargin: 10
        source: "qrc:/share/bibletime/icons/bibletime.svg"
    }

    Text {
        id: title
        color: "black"
        font.pointSize: btStyle.uiFontPointSize
        anchors.left: logo.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: 20
        verticalAlignment: Text.AlignVCenter
        text: "BibleTime Mobile"
    }

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
