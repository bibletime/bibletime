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

    signal resultsFinished();
    signal resultsMenuRequested();
    signal progressTextChanged(string text);
    signal progressValueChanged(int value);

    function cancel() {
        btSearchInterface.cancel();
        indexingCancelled = true;
    }

    function indexingWasCancelled() {
        return indexingCancelled;
    }

    function getModule() {
        return btWindowInterface.moduleName;
    }

    function getReference() {
        return btWindowInterface.reference;
    }

    orientation: Qt.Vertical
    handleDelegate: Rectangle {
        width: 1;
        height: {
            var pixel = btStyle.pixelsPerMillimeterY * 7;
            var uiFont = btStyle.uiFontPointSize * 3;
            var mix = pixel * 0.7 + uiFont * 0.3;
            return Math.max(pixel, mix);
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
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: searchMenuButton.left
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        MenuButton {
            id: searchMenuButton

            width: parent.height * 1.1
            height: parent.height
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 2
            foreground: btStyle.toolbarTextColor
            background: btStyle.toolbarColor
            visible: btSearchInterface.haveReferences

            onButtonClicked: {
                resultsMenuRequested();
            }
        }
    }

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) &&
                screenView.currentIndex == screenModel.results) {
            resultsFinished();
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

        Rectangle {
            id: searchResultsTitleBar
            color: btStyle.toolbarColor
            width: parent.width
            height: btStyle.pixelsPerMillimeterY * 8

            Back {
                id: backTool

                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                text: qsTranslate("Search", "Main")
                onClicked: {
                    searchResults.resultsFinished();
                }
            }

            Text {
                id: title2
                color: btStyle.toolbarTextColor
                font.pointSize: btStyle.uiFontPointSize
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.leftMargin: search.spacing
                verticalAlignment: Text.AlignVCenter
                text: qsTranslate("SearchResults", "Search Results")
            }
        }

        SplitView {
            id: topSplitter

            orientation: Qt.Horizontal
            anchors.top: searchResultsTitleBar.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
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

        onProgressTextChanged: {
            searchResults.progressTextChanged(btSearchInterface.progressText);
        }
        onProgressValueChanged: {
            searchResults.progressValueChanged(btSearchInterface.progressValue)
        }
    }
}
