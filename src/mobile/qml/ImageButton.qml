import QtQuick 2.0

Rectangle {
    id: imageButton

    property bool show: true
    property string icon: ""
    property int corner: 6

    border.width: 1
    border.color: "gray"
    radius: corner

    Image{
        id: nextIcon
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        source: imageButton.icon
        height: parent.height
        width: parent.height
        anchors.right: parent.right
        anchors.top: parent.top
    }
}
