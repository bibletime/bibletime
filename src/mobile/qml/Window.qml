import QtQuick 2.0
import QtWebKit 3.0
import BibleTime 1.0

Rectangle {
    id: htmlView

    property string title: toolbar.title

    function changeModule() {
        btWindowInterface.changeModule();
    }

    function setModule(module) {
        btWindowInterface.moduleName = module;
    }

    color: "black"

    BtWindowInterface {
        id: btWindowInterface

        onTextChanged: {
            webView.loadHtml(btWindowInterface.text)
        }
    }

    BtStyle {
        id:btStyle
    }

    Rectangle {
        id: toolbar

        property string title: btWindowInterface.displayed + " (" + btWindowInterface.moduleName + ")"

        width: parent.width
        height: 36
        color: btStyle.toolbarColor

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
                font.pointSize: btStyle.toolbarTextPointSize
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

            function createDialog() {
                btWindowInterface.changeReference()
            }

            width:referenceText.width + 10
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
                font.pointSize: btStyle.toolbarTextPointSize
                color: btStyle.toolbarButtonText
                text: btWindowInterface.displayed
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
