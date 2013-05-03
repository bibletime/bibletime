import QtQuick 2.0
import QtWebKit 3.0
import BibleTime 1.0

Rectangle {
    id: htmlView

    property string title: toolbar.title

    function changeModule() {
        btBookInterface.changeModule();
    }

    color: "black"

    BtBookInterface {
        id: btBookInterface

        onTextChanged: {
            webView.loadHtml(btBookInterface.text)
        }
    }

    BtStyle {
        id:btStyle
    }

    Rectangle {
        id: toolbar

        property string title: btBookInterface.displayed + " (" + btBookInterface.moduleName + ")"

        width: parent.width
        height: 36
        color: btStyle.toolbar

        Rectangle {
            id: moduleDisplay

            width: text.width +10
            radius:btStyle.buttonRadius
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: 2
            anchors.leftMargin: 5
            anchors.bottomMargin: 2
            color: btStyle.toolbarButton
            border.color: btStyle.buttonBorder
            border.width: 1

            Text {
                id: text

                anchors.centerIn: parent
                font.pointSize: 11
                elide: Text.ElideMiddle
                color: btStyle.toolbarButtonText
                text: btBookInterface.moduleName
            }

            MouseArea {
                id: moduleMouseArea

                anchors.fill: parent
                onClicked: {
                    btBookInterface.changeModule();
                }
            }
        }

        Rectangle {
            id: referenceDisplay

            function createDialog() {
                btBookInterface.chooseKey()
            }

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
            anchors.topMargin: 2
            anchors.bottomMargin: 2
            anchors.leftMargin: 5
            color: btStyle.toolbarButton
            border.color: btStyle.buttonBorder
            border.width: 1

            Text {
                id: referenceText
                anchors.centerIn: parent
                width: referenceDisplay.width
                font.pointSize: 11
                elide: Text.ElideMiddle
                color: btStyle.toolbarButtonText
                text: btBookInterface.displayed
            }

            MouseArea {
                id: mouseArea

                anchors.fill: parent
                onClicked: {
                    referenceDisplay.createDialog()
                }
            }
        }
    }

    WebView {
        id: webView

        anchors.top: toolbar.bottom
        anchors.bottom: htmlView.bottom
        anchors.left: htmlView.left
        anchors.right: htmlView.right
    }
}
