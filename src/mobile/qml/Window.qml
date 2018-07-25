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
import QtQuick.Controls 2.2
import BibleTime 1.0

Rectangle {
    id: windowView

    property string title: toolbar.title
    property int currentModule: 1
    property variant textModel: btWindowInterface.textModel
    property bool toolbarEnabled: true
    property alias reference: btWindowInterface.reference

    onToolbarEnabledChanged: {
        menuButton.visible = windowView.toolbarEnabled;
    }

    border.color: btStyle.toolbarTextColor

    signal windowMenusDialog(variant window)

    signal moduleChooserRequest(variant window, int moduleNumber)

    function referenceChoosen() {
        chooseReference.finished.disconnect(windowView.referenceChoosen);
        btWindowInterface.reference = chooseReference.reference
        btWindowInterface.referenceChosen();
    }

    function lexiconReferenceChoosen(index) {
        chooseReference.finishedLexicon.disconnect(windowView.lexiconReferenceChoosen);
        btWindowInterface.lexiconReferenceChoosen(index);
    }

    function addParallelModule() {
        moduleChooser.bibleCommentaryOnly = true;
        chooseModule(btWindowInterface.numModules+1);
    }

    function removeParallelModule() {
        moduleChooser.bibleCommentaryOnly = false;
        var n = btWindowInterface.numModules
        if (n == 4)
            btWindowInterface.module4Name = "";
        if (n == 3)
            btWindowInterface.module3Name = "";
        if (n == 2)
            btWindowInterface.module2Name = "";
    }

    function chooseModule(moduleNumber) {
        windowView.currentModule = moduleNumber;
        moduleChooser.moduleSelected.connect(windowView.moduleChoosenSlot);
        moduleChooser.backText = qsTranslate("Navigation", "Main");
        moduleChooser.visible = true;
    }

    function moduleChoosenSlot() {
        moduleChooser.moduleSelected.disconnect(windowView.moduleChoosenSlot);
        var moduleName = moduleChooser.selectedModule;
        if (windowView.currentModule == 1)
            btWindowInterface.moduleName = moduleName;
        else if (windowView.currentModule == 2)
            btWindowInterface.module2Name = moduleName;
        else if (windowView.currentModule == 3)
            btWindowInterface.module3Name = moduleName;
        else if (windowView.currentModule == 4)
            btWindowInterface.module4Name = moduleName;
    }

    function createMenus(model) {
        model.clear();
        model.append ({ title: qsTr("Add BookMark"), action: "addBookmark" })
        model.append ({ title: qsTr("Bookmarks"), action: "bookmarks" })
        if (btWindowInterface.firstModuleIsBibleOrCommentary() ||
                btWindowInterface.firstModuleIsBook()) {
            model.append ({ title: qsTr("Copy"), action: "copy" })
        }
        if (btWindowInterface.firstModuleIsBibleOrCommentary()) {
            model.append ({ title: qsTr("View References"),    action: "viewReferences" })
            if (btWindowInterface.numModules < 4)
                model.append ({ title: qsTr("Add Parallel Document"), action: "addParallel" })
            if (btWindowInterface.numModules > 1)
                model.append ({ title: qsTr("Remove Parallel Document"), action: "removeParallel" })
        }
        model.append ({ title: qsTr("Close Window"), action: "close window" })
        //model.append ({ title: "Debug Data", action: "debugData" })
    }

    function getModuleLanguage() {
        return btWindowInterface.moduleLanguage
    }

    function getModuleNames() {
        return btWindowInterface.getModuleNames();
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

    function getEnglishReference() {
        return btWindowInterface.englishReference;
    }

    function setModule(module) {
        btWindowInterface.moduleName = module;
    }

    function setModules(modules) {
        btWindowInterface.moduleName = modules[0];
        if (modules.length > 1) {
            btWindowInterface.module2Name = modules[1];
            moduleChooser.bibleCommentaryOnly = true;
        }
        if (modules.length > 2) {
            btWindowInterface.module3Name = modules[2];
        }
        if (modules.length > 3) {
            btWindowInterface.module4Name = modules[3];
        }
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
        if (btWindowInterface.numModules >1)
            toolbar.title += " / " + btWindowInterface.module2Name;
        if (btWindowInterface.numModules >2)
            toolbar.title += " / " + btWindowInterface.module3Name;
        if (btWindowInterface.numModules >3)
            toolbar.title += " / " + btWindowInterface.module4Name;

        toolbar.title += " (" + btWindowInterface.reference + ")";
    }

    function debugData() {
        var i = 31472;
        var text = btWindowInterface.getModelTextByIndex(i);
        console.log(text);
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
            var pixel = btStyle.pixelsPerMillimeterY * 7;
            var uiFont = btStyle.uiFontPointSize * 3;
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
            visible: {
                btWindowInterface.numModules > 1;
            }
            onActivated: {
                moduleChooser.bibleCommentaryOnly = btWindowInterface.module2Name.length > 0;
                windowView.chooseModule(2);
            }
        }

        ModuleDisplay {
            id: moduleDisplay3
            anchors.left: moduleDisplay2.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: btStyle.pixelsPerMillimeterY * 0.7
            anchors.leftMargin: parent.height * 0.1
            anchors.bottomMargin: btStyle.pixelsPerMillimeterY * 0.7
            moduleText: btWindowInterface.module3Name
            visible: {
                btWindowInterface.numModules > 2;
            }
            onActivated: {
                moduleChooser.bibleCommentaryOnly = btWindowInterface.module2Name.length > 0;
                windowView.chooseModule(3);
            }
        }

        ModuleDisplay {
            id: moduleDisplay4
            anchors.left: moduleDisplay3.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: btStyle.pixelsPerMillimeterY * 0.7
            anchors.leftMargin: parent.height * 0.1
            anchors.bottomMargin: btStyle.pixelsPerMillimeterY * 0.7
            moduleText: btWindowInterface.module4Name
            visible: {
                btWindowInterface.numModules > 3;
            }
            onActivated: {
                moduleChooser.bibleCommentaryOnly = btWindowInterface.module2Name.length > 0;
                windowView.chooseModule(4);
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
            anchors.left: moduleDisplay4.right
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
                anchors.leftMargin: btStyle.pixelsPerMillimeterX * 1
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
                    setHistoryPoint();
                    var moduleName = btWindowInterface.moduleName;
                    var reference = btWindowInterface.reference;

                    if (btWindowInterface.firstModuleIsBibleOrCommentary() ||
                            btWindowInterface.firstModuleIsBook()) {
                        chooseReference.finished.connect(windowView.referenceChoosen);
                        chooseReference.start(moduleName, reference,qsTranslate("Navigation", "Main"));
                    } else {
                        chooseReference.finishedLexicon.connect(windowView.lexiconReferenceChoosen);
                        chooseReference.startLexicon(moduleName,
                                                     reference,
                                                     btWindowInterface.textModel,
                                                     btWindowInterface.currentModelIndex);
                    }
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

            property int columns: btWindowInterface.numModules
            property int savedRow: 0
            property int savedColumn: 0

            function startEdit(row, column) {
                if (!btWindowInterface.moduleIsWritable(column))
                    return
                savedRow = row;
                savedColumn = column;
                var rawText = btWindowInterface.getRawText(row, column);
                textEditor.editFinished.connect( listView.finishEdit);
                textEditor.open(rawText);
            }

            function finishEdit(newText) {
                btWindowInterface.setRawText(savedRow, savedColumn, newText)
            }

            function updateReferenceText() {
                var index = indexAt(contentX,contentY+30);
                btWindowInterface.updateKeyText(index);
            }

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
                updateReferenceText();
            }

            delegate: Component {
                Rectangle {
                    color: btStyle.textBackgroundColor
                    width: listView.width
                    height: {
                        if (listView.columns == 1)
                            return column0Text.height
                        if (listView.columns == 2)
                            return Math.max(column0Text.height, column1Text.height)
                        if (listView.columns == 3)
                            return Math.max(column0Text.height, column1Text.height, column2Text.height)
                        if (listView.columns == 4)
                            return Math.max(column0Text.height, column1Text.height,
                                            column2Text.height, column3Text.height)
                        return 30;
                    }

                    Text {
                        id: column0Text
                        text: text1
                        textFormat: Text.RichText
                        anchors.top: parent.top
                        anchors.left: parent.left
                        width: listView.width / listView.columns
                        color: btStyle.textColor
                        font.family: btWindowInterface.fontName
                        font.pointSize: btWindowInterface.fontSize
                        wrapMode: Text.WordWrap
                        visible: listView.columns > 0
                        onWidthChanged: doLayout()

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                listView.startEdit(index, 0);
                            }
                        }
                    }

                    Text {
                        id: column1Text
                        text: text2
                        textFormat: Text.RichText
                        anchors.top: parent.top
                        anchors.left: column0Text.right
                        anchors.leftMargin: btStyle.pixelsPerMillimeterX * 0.8
                        width: listView.width / listView.columns
                        color: btStyle.textColor
                        font.family: btWindowInterface.fontName
                        font.pointSize: btWindowInterface.fontSize
                        wrapMode: Text.WordWrap
                        visible: listView.columns > 1
                        onWidthChanged: doLayout()

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                listView.startEdit(index, 1);
                            }
                        }
                    }

                    Text {
                        id: column2Text
                        text: text3
                        textFormat: Text.RichText
                        anchors.top: parent.top
                        anchors.left: column1Text.right
                        anchors.leftMargin: btStyle.pixelsPerMillimeterX * 0.8
                        width: listView.width / listView.columns
                        color: btStyle.textColor
                        font.family: btWindowInterface.fontName
                        font.pointSize: btWindowInterface.fontSize
                        wrapMode: Text.WordWrap
                        visible: listView.columns > 2
                        onWidthChanged: doLayout()

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                listView.startEdit(index, 2);
                            }
                        }
                    }

                    Text {
                        id: column3Text
                        text: text4
                        textFormat: Text.RichText
                        anchors.top: parent.top
                        anchors.left: column2Text.right
                        anchors.leftMargin: btStyle.pixelsPerMillimeterX * 0.8
                        width: listView.width / listView.columns
                        color: btStyle.textColor
                        font.family: btWindowInterface.fontName
                        font.pointSize: btWindowInterface.fontSize
                        wrapMode: Text.WordWrap
                        visible: listView.columns > 3
                        onWidthChanged: doLayout()

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                listView.startEdit(index, 3);
                            }
                        }
                    }

                }
            }
        }

        ScrollBar {
            property int savedY: 0
            property int factor: 20
            property real savedPosition: 0
            property bool firstPositionChanged: false
            width: btStyle.pixelsPerMillimeterX * 7
            anchors.top: listView.top
            anchors.bottom: listView.bottom
            anchors.right: listView.right
            position: 0.5
            size: 0.1
            onPositionChanged: {
                if (firstPositionChanged) {
                    savedPosition = position;
                    firstPositionChanged = false;
                }
                var pixel = (position - savedPosition) * height;
                var Y = savedY + (pixel * factor)
                if (Y < 0)
                    Y = 0;
                listView.contentY = Y;
                listView.updateReferenceText();
            }
            onPressedChanged: {
                if (pressed) {
                    savedPosition = position;
                    savedY = listView.contentY
                    firstPositionChanged = true;
                } else {

                    savedY = listView.contentY;
                    savedPosition = 0.5;
                    position = 0.5;
                }
            }
        }
    }
}
