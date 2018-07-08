
import QtQuick 2.3

Item {
    id: backTool

    property string text: ""

    signal clicked

    Left {
        id: leftSymbol

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        background: btStyle.toolbarColor
        textColor: btStyle.toolbarTextColor
        borderColor: btStyle.toolbarTextColor
        width: parent.height * 0.50
        height: parent.height * 0.85
    }

    Text {
        id: backText
        color: btStyle.toolbarTextColor
        font.pointSize: btStyle.uiFontPointSize
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: search.spacing
        anchors.left: leftSymbol.right
        verticalAlignment: Text.AlignVCenter
        text: backTool.text
    }

    MouseArea {
        anchors.left: parent.left
        anchors.right: backText.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        onClicked: {
            backTool.clicked();
        }
    }
}


