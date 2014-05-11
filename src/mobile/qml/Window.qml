/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: windowView

    property string title: toolbar.title

    function getModule() {
        return btWindowInterface.moduleName;
    }

    function getReference() {
        return btWindowInterface.reference;
    }

    function setModule(module) {
        btWindowInterface.moduleName = module;
    }

    function setKey(key) {
        btWindowInterface.reference = key;
        btWindowInterface.updateCurrentModelIndex();
    }

    function saveWindowStateToConfig (index) {
        btWindowInterface.saveWindowStateToConfig(index);
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
        height: btStyle.pixelsPerMillimeterY * 8
        color: btStyle.toolbarColor
        border.width: 1
        border.color: "black"

        Rectangle {
            id: moduleDisplay

            width: text.width + 30
            radius:btStyle.buttonRadius
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: btStyle.pixelsPerMillimeterY * 0.7
            anchors.leftMargin: 15
            anchors.bottomMargin: btStyle.pixelsPerMillimeterY * 0.7
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
                var w1 = 1100
                var w2 = toolbar.width - moduleDisplay.width - 30;
                var w = Math.min(w1,w2);
                return w;
            }
            radius: btStyle.buttonRadius
            anchors.left: moduleDisplay.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: btStyle.pixelsPerMillimeterY * 0.7
            anchors.bottomMargin: btStyle.pixelsPerMillimeterY * 0.7
            anchors.leftMargin: 10
            color: btStyle.toolbarButton
            border.color: btStyle.buttonBorder
            border.width: 1

            Text {
                id: referenceText
                anchors.fill: parent
                anchors.leftMargin: 15
                anchors.rightMargin: 4
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
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
            highlightFollowsCurrentItem: true
            currentIndex: btWindowInterface.currentModelIndex
            onCurrentIndexChanged: {
                positionViewAtIndex(currentIndex,ListView.Beginning)
            }
            onMovementEnded: {
                var index = indexAt(contentX,contentY+30);
                btWindowInterface.updateKeyText(index);
            }

            delegate: Text {
                text: line
                textFormat: Text.RichText
                width: parent.width
                color: "black"
                font.pointSize: btStyle.textFontPointSize
                wrapMode: Text.WordWrap
                onWidthChanged: doLayout()
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

        ListElement { title: QT_TR_NOOP("Text Font Size"); action: "textSize" }
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
