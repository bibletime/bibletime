import QtQuick 2.0

Column {
    id: menus

    width:120
    height:20 * listModel.count
    anchors.top: mainToolbar.bottom
    anchors.right: mainToolbar.right
    z: 100
    visible: false

    Repeater {
        model: listModel
        delegate: Rectangle {
            width:parent.width
            height:24
            color: btStyle.menu
            border.color: btStyle.menuBorder
            border.width: 1
            Text {
                anchors.centerIn: parent
                text: title
                color: btStyle.menuText
            }
        }
    }
}
