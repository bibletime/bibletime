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
import BibleTime 1.0

Rectangle {
    id: windowView

    property string title: toolbar.title
    property int currentModule: 1
    property bool parallelView: btWindowInterface.module2Name != ""
    border.color: btStyle.toolbarTextColor

    signal windowMenusDialog(variant window)

    signal moduleChooserRequest(variant window, int moduleNumber)

    function addParallelModule() {
        moduleChooser.bibleCommentaryOnly = true;
        moduleDisplay2.visible = true;
        chooseModule(2);
    }

    function removeParallelModule() {
        moduleDisplay2.visible = false;
        btWindowInterface.module2Name = "";
    }

    function chooseModule(moduleNumber) {
        windowView.currentModule = moduleNumber;
        moduleChooser.moduleSelected.connect(windowView.moduleChoosenSlot);
        moduleChooser.visible = true;
    }

    function moduleChoosenSlot() {
        moduleChooser.moduleSelected.disconnect(windowView.moduleChoosenSlot);
        var moduleName = moduleChooser.selectedModule;
        if (windowView.currentModule == 1)
            btWindowInterface.moduleName = moduleName;
        else
            btWindowInterface.module2Name = moduleName;
    }

    function createMenus(model) {
        model.clear();
        model.append ({ title: QT_TR_NOOP("Add BookMark"), action: "addBookmark" })
        model.append ({ title: QT_TR_NOOP("Bookmarks"), action: "bookmarks" })
        model.append ({ title: QT_TR_NOOP("View References"),    action: "viewReferences" })
        if (btWindowInterface.module2Name === "")
            model.append ({ title: QT_TR_NOOP("Add Parallel Document"), action: "addParallel" })
        else
            model.append ({ title: QT_TR_NOOP("Remove Parallel Document"), action: "removeParallel" })
        model.append ({ title: QT_TR_NOOP("Close Window"), action: "close window" })
    }

    function getModuleLanguage() {
        return btWindowInterface.moduleLanguage
    }

    function updateTextFonts() {
        btWindowInterface.updateTextFonts();
    }

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

    function setHistoryPoint() {
        btWindowInterface.setHistoryPoint();
    }

    function setModuleToBeginning() {
        btWindowInterface.setModuleToBeginning();
    }

    function formatTitle() {
        toolbar.title = btWindowInterface.moduleName;
        if (btWindowInterface.module2Name != "") {
            toolbar.title += " / " + btWindowInterface.module2Name;
        }
        toolbar.title += " (" + btWindowInterface.reference + ")";
    }

    color: "black"

    BtWindowInterface {
        id: btWindowInterface

        isMagView: false
        onModuleChanged: formatTitle();
        onModule2Changed: formatTitle();
        onReferenceChanged: formatTitle();
    }

    BtStyle {
        id:btStyle
    }

    Rectangle {
        id: toolbar

        property string title: ""

        height: {
            var pixel = btStyle.pixelsPerMillimeterY * 8;
            var uiFont = btStyle.uiFontPointSize * 4.4;
            var mix = pixel * 0.7 + uiFont * 0.3;
            return Math.max(pixel, mix);
        }
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 1


        color: btStyle.toolbarColor

        PrevNextArrow {
            id: prevHistory

            background: btStyle.toolbarColor
            textColor: btStyle.toolbarTextColor
            borderColor: btStyle.toolbarTextColor
            prev: true
            width: parent.height * 0.80
            height: parent.height * 0.80
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: parent.height * 0.1;
            show: btWindowInterface.historyBackwardVisible
            onClicked: {
                if (show)
                    btWindowInterface.moveHistoryBackward();
            }
        }

        PrevNextArrow {
            id: nextHistory

            background: btStyle.toolbarColor
            textColor: btStyle.toolbarTextColor
            borderColor: btStyle.toolbarTextColor
            prev: false
            width: parent.height * 0.80
            height: parent.height * 0.80
            anchors.left: prevHistory.right
            anchors.top: parent.top
            anchors.margins: parent.height * 0.1;
            show: btWindowInterface.historyForwardVisible
            onClicked: {
                if (show)
                    btWindowInterface.moveHistoryForward();
            }
        }

        ModuleDisplay {
            id: moduleDisplay
            anchors.left: nextHistory.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: btStyle.pixelsPerMillimeterY * 0.7
            anchors.leftMargin: parent.height * 0.1
            anchors.bottomMargin: btStyle.pixelsPerMillimeterY * 0.7
            moduleText: btWindowInterface.moduleName
            visible: true
            onActivated: {
                moduleChooser.bibleCommentaryOnly = btWindowInterface.module2Name.length > 0;
                windowView.chooseModule(1);
            }
        }

        ModuleDisplay {
            id: moduleDisplay2
            anchors.left: moduleDisplay.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: btStyle.pixelsPerMillimeterY * 0.7
            anchors.leftMargin: parent.height * 0.1
            anchors.bottomMargin: btStyle.pixelsPerMillimeterY * 0.7
            moduleText: btWindowInterface.module2Name
            visible: false
            onActivated: {
                moduleChooser.bibleCommentaryOnly = btWindowInterface.module2Name.length > 0;
                windowView.chooseModule(2);
            }
        }

        MenuButton {
            id: menuButton

            width: parent.height * 1.1
            height: parent.height
            anchors.right: parent.right
            anchors.top: parent.top
            foreground: btStyle.toolbarTextColor
            background: btStyle.toolbarColor

            onButtonClicked: {
                windowMenusDialog(windowView);
            }
        }

        Rectangle {
            id: referenceDisplay

            radius: btStyle.pixelsPerMillimeterX
            anchors.left: moduleDisplay2.right
            anchors.right: menuButton.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: btStyle.pixelsPerMillimeterY * 0.7
            anchors.bottomMargin: btStyle.pixelsPerMillimeterY * 0.7
            anchors.leftMargin:  parent.height * 0.1
            color: btStyle.textBackgroundColor
            border.color: btStyle.toolbarTextColor
            border.width: 2

            Text {
                id: referenceText
                anchors.fill: parent
                anchors.leftMargin: btStyle.pixelsPerMillimeterX *4.5
                anchors.rightMargin: 4
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.pointSize: btStyle.uiFontPointSize
                elide: Text.ElideMiddle
                color: btStyle.textColor
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

        color: btStyle.textBackgroundColor
        anchors.top: toolbar.bottom
        anchors.left: windowView.left
        anchors.right: windowView.right
        anchors.bottom: windowView.bottom
        anchors.margins: 2

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
                width: listView.width
                color: btStyle.textColor
                font.family: btWindowInterface.fontName
                font.pointSize: btWindowInterface.fontSize
                wrapMode: Text.WordWrap
                onWidthChanged: doLayout()
            }
        }
    }
}
