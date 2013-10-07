import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: windowView

    property string title: toolbar.title

    function setModule(module) {
        btWindowInterface.moduleName = module;
    }

    function contextMenus() {
//        contextMenu.visible = true;
    }

    color: "black"

    BtWindowInterface {
        id: btWindowInterface
    }

    BtStyle {
        id:btStyle
    }

    Rectangle {
        id: toolbar

        property string title: btWindowInterface.moduleName + " (" + btWindowInterface.reference + ")"

        width: parent.width
        height: 36
        color: btStyle.toolbarColor
        border.width: 1
        border.color: "black"

        Rectangle {
            id: moduleDisplay

            width: text.width +10
            radius:btStyle.buttonRadius
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: 4
            anchors.leftMargin: 5
            anchors.bottomMargin: 4
            color: btStyle.toolbarButton
            border.color: btStyle.buttonBorder
            border.width: 1

            Text {
                id: text

                anchors.centerIn: parent
                anchors.leftMargin: 4
                anchors.rightMargin: 4
                font.pointSize: btStyle.uiFontPointSize
                elide: Text.ElideMiddle
                color: btStyle.toolbarButtonText
                text: btWindowInterface.moduleName
            }

            MouseArea {
                id: moduleMouseArea

                anchors.fill: parent
                onClicked: {
                    btWindowInterface.changeModule();
                }
            }
        }


        Rectangle {
            id: referenceDisplay

            width: {
                var w1 = 300
                var w2 = toolbar.width - moduleDisplay.width;
                var w = Math.min(w1,w2);
                return w - 15;
            }
            radius: btStyle.buttonRadius
            anchors.left: moduleDisplay.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: 4
            anchors.bottomMargin: 4
            anchors.leftMargin: 5
            color: btStyle.toolbarButton
            border.color: btStyle.buttonBorder
            border.width: 1

            Text {
                id: referenceText
                anchors.centerIn: parent
                anchors.leftMargin: 6
                anchors.rightMargin: 4
                width: referenceDisplay.width - 4
                font.pointSize: btStyle.uiFontPointSize
                elide: Text.ElideMiddle
                color: btStyle.toolbarButtonText
                text: btWindowInterface.reference
            }

            MouseArea {
                id: mouseArea

                anchors.fill: parent
                onClicked: {
                    btWindowInterface.changeReference();
                }
            }
        }

    }

    Rectangle {
        id: mainTextView

        color: "white"
        anchors.top: toolbar.bottom
        anchors.left: windowView.left
        anchors.right: windowView.right
        anchors.bottom: windowView.bottom

        ListView {
            id: listView

            clip: true
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            model: btWindowInterface.textModel
            currentIndex: btWindowInterface.currentModelIndex
            delegate: Text {
                text: "<font color=\"blue\">" + ref + "</font> " + line
                width: parent.width
                color: "black"
                font.pointSize: btStyle.textFontPointSize
                wrapMode: Text.WordWrap
            }

            MouseArea {

                anchors.fill: parent
                onDoubleClicked: {
                    windowView.contextMenus();
                }

                onPressAndHold: {
                    windowView.contextMenus();

                }
            }
        }
    }

    ListModel {
        id: contextMenuModel

        ListElement { title: "Text Font Size"; action: "textSize" }
    }

    ContextMenu {
        id: contextMenu

        function doAction(action) {
        }

        model: contextMenuModel
        visible: false
        Component.onCompleted: contextMenu.accepted.connect(contextMenu.doAction)
    }

}
