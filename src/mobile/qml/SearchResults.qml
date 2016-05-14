/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2016 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import BibleTime 1.0

SplitView {
    id: searchResults

    property alias searchText: btSearchInterface.searchText
    property alias findChoice: btSearchInterface.findChoice
    property alias moduleList: btSearchInterface.moduleList
    property alias indexingFinished: btSearchInterface.indexingFinished

    property real leftRightSplit: 0.35
    property real topBottomSplit: 0.45
    property real handleWidth: btStyle.pixelsPerMillimeterX * 3
    property bool indexingCancelled;

    function cancel() {
        btSearchInterface.cancel();
        indexingCancelled = true;
    }

    function indexingWasCancelled() {
        return indexingCancelled;
    }

    orientation: Qt.Vertical
    handleDelegate: Rectangle {
        width: 1;
        height: {
            var pixel = btStyle.pixelsPerMillimeterY * 6;
            var uiFont = btStyle.uiFontPointSize * 2.0;
            return Math.max(pixel, uiFont);
        }

        color: btStyle.toolbarColor
        border.color: btStyle.textColor
        border.width: 2

        Text {
            id: title
            text: {
                return btWindowInterface.moduleName + "   " +  btWindowInterface.reference;
            }
            color: btStyle.toolbarTextColor
            font.pointSize: btStyle.uiFontPointSize
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    anchors.fill: parent
    visible: false

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && searchResults.visible == true) {
            searchResults.visible = false;
            event.accepted = true;
        }
    }

    function modulesAreIndexed() {
        return btSearchInterface.modulesAreIndexed();
    }

    function indexModules() {
        indexingCancelled = false;
        return btSearchInterface.indexModules();
    }

    function performSearch() {
        btWindowInterface.highlightWords = btSearchInterface.searchText;
        btSearchInterface.performSearch();
        modulesView.currentIndex = 0;
        referencesView.currentIndex = 0;
        referencesView.updateTextDisplay();
        Qt.inputMethod.hide();
    }

    Rectangle {
        id: topArea

        width: parent.width
        height: parent.height * searchResults.topBottomSplit
        color: btStyle.textBackgroundColor

        SplitView {
            id: topSplitter

            orientation: Qt.Horizontal
            anchors.fill: parent
            handleDelegate: Rectangle {
                width: handleWidth;
                height: 2;
                color: btStyle.toolbarColor
                border.color: "black"
                border.width: 2
            }

            Rectangle {
                id: modules

                color: btStyle.textBackgroundColor
                anchors.left: parent.left
                anchors.top: parent.top
                width: parent.width * searchResults.leftRightSplit
                height: parent.height

                TitleColorBar {
                    id: titleBar1

                    title: qsTr("Documents")
                    width: parent.width
                    anchors.left: parent.left
                    anchors.top: parent.top
                }

                ListSelectView {
                    id: modulesView

                    model: btSearchInterface.modulesModel
                    anchors.top: titleBar1.bottom
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    leftTextMargin: 30

                    onItemSelected: {
                        btSearchInterface.selectReferences(currentIndex);
                    }
                }
            }

            Rectangle {
                id: references

                color: "white"
                anchors.top: parent.top
                Layout.fillWidth: true

                TitleColorBar {
                    id: titleBar2
                    title: qsTr("References")
                    width: parent.width
                }

                ListSelectView {
                    id: referencesView

                    model: btSearchInterface.referencesModel
                    anchors.top: titleBar2.bottom
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    leftTextMargin: 30

                    onItemSelected: {
                        updateTextDisplay()
                    }

                    function updateTextDisplay() {
                        var moduleName = btSearchInterface.getModuleName(modulesView.currentIndex);
                        var reference = btSearchInterface.getReference(referencesView.currentIndex);
                        btWindowInterface.moduleName = moduleName;
                        btWindowInterface.reference = reference;
                        btWindowInterface.updateCurrentModelIndex();
                    }
                }
            }
        }
    }

    Rectangle {
        id: text

        color: btStyle.textBackgroundColor
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: parent.width
        Layout.fillWidth: true

        ListView {
            id: listView

            clip: true
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
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
                color: btStyle.textColor
                font.family: btWindowInterface.fontName
                font.pointSize: btWindowInterface.fontSize
                wrapMode: Text.WordWrap
                onWidthChanged: doLayout()
            }
        }
    }

    BtWindowInterface {
        id: btWindowInterface
    }

    BtSearchInterface {
        id: btSearchInterface
    }
}
