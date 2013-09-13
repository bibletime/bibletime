import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: settings

    property int finalHeight: 300

    color: "white"
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.right: parent.right
    height: parent.height

    onVisibleChanged: PropertyAnimation {
        target: settings
        property: "opacity"
        from: 0
        to: 1
        duration: 400
        easing.type: Easing.InOutCubic
    }

    BtStyle {
        id: btStyle
    }

    ListModel {
        id: settingsModel

        ListElement { title: "Ui Font Size";   action: "uiSize" }
    }

    ListView {
        id: settingsList

        anchors.fill: parent
        anchors.topMargin: 100
        model: settingsModel

        delegate: Rectangle {
            color: "white"
            border.color: "lightgray"
            border.width: 1
            width: parent.width
            height: children[0].height * 2.5

            Text {
                id: menuText
                x: 40
                anchors.verticalCenter: parent.verticalCenter
                text: title
                color: "black"
                font.pointSize: btStyle.uiTextPointSize
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (action == "uiSize")
                        uiFontPointSize.visible = true;
                }
            }
        }
    }

    ImageButton {
        id: backButton

        icon: "leftarrow.svg"
        height: 36
        width:  56
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 8
        visible: true

        MouseArea {
            anchors.fill: parent
            onClicked: {
                settings.visible = false;
            }
        }
    }

    FontSizeSlider {
        id: uiFontPointSize
        visible: false
        title: "UI Font Point Size"

        onVisibleChanged: {
            if (visible)
                uiFontPointSize.current = btStyle.uiTextPointSize
        }

        onAccepted: {
            btStyle.uiTextPointSize = uiFontPointSize.current;
            settings.visible = false;
        }
    }
}
