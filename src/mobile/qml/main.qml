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

import QtQuick 2.2
import QtQml 2.2
import QtQuick.Window 2.1
import BibleTime 1.0

Rectangle {
    id: root

    color: "blue"
    property int opacitypopup: 0
    property QtObject component: null;
    property Item window: null;

    Component.onCompleted: {
        setFontDialog.textFontChanged.connect(windowManager.updateTextFont)
    }

    ListModel {
        id: viewWindowsModel

        ListElement { title: ""; action: "" }
    }

    ListModel {
        id: closeWindowsModel

        ListElement { title: ""; action: "" }
    }

    function installModules() {
        installManager.openChooser();
    }

    function startSearch() {
        var moduleNames = windowManager.getUniqueModuleNames();
        searchModel.appendModuleChoices(moduleNames);
        search.searchText = "";
        search.visible = true;
    }

    Keys.forwardTo: [
        gridChooser,
        moduleChooser,
        mainMenus,
        windowArrangementMenus,
        viewWindowsMenus,
        closeWindowsMenus,
        searchResults,
        search,
        installManagerChooser,
        keyNameChooser,
        treeChooser,
        aboutDialog,
        uiFontPointSize,
        setFontDialog
    ]

    Keys.onReleased: {
        if (event.key == Qt.Key_Back || event.key == Qt.Key_Escape) {
            event.accepted = true;
            quitQuestion.visible = true;
        }
    }

    Keys.onMenuPressed: {
        event.accepted = true;
        mainMenus.visible = ! mainMenus.visible
    }

    width:  480
    height: 800

    rotation: 0

    MainToolbar {
        id: mainToolbar

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        height: {
            var pixel = btStyle.pixelsPerMillimeterY * 7.5;
            var uiFont = btStyle.uiFontPointSize * 4.4;
            var mix = pixel * 0.7 + uiFont * 0.3;
            return Math.max(pixel, mix);
        }
        onButtonClicked: {
            mainMenus.visible = ! mainMenus.visible;
        }
        onSearchClicked: {
            startSearch();
        }
    }

    Rectangle {
        id: spacer

        anchors.top: mainToolbar.bottom
        height:2
        width: parent.width
        color: "#646464"
    }

    WindowManager {
        id: windowManager

        anchors.top: spacer.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: btStyle.toolbarColor

    }

    Settings {
        id: settings

        visible: false;
    }

    GridChooser {
        id: gridChooser

        objectName: "gridChooser"
        width: parent.width
        height: parent.height
        visible: false
    }

    BtStyle {
        id: btStyle
    }

    ModuleChooser {
        id: moduleChooser

        objectName: "moduleChooser"
        visible: false
        width: Math.min(parent.height, parent.width);
        height: parent.height
        anchors.centerIn: parent
    }

    TreeChooser {
        id: treeChooser

        objectName: "treeChooser"
        width:parent.width
        height: parent.height
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        path: ""
        visible: false
        z: 100
    }

    KeyNameChooser {
        id: keyNameChooser

        objectName: "keyNameChooser"
        width: parent.width
        height: parent.height
        anchors.top: parent.top
        visible: false
    }

    StartupBookshelfManager {
        id: startupBookshelfManager

        objectName: "startupBookshelfManager"
        visible: false
        width: windowManager.width
        height: windowManager.height
        anchors.centerIn: windowManager
        onBookshelfRequested: {
            installModules();
        }
    }

    Search {
        id: search

        moduleChoices: searchModel
        onSearchRequest: {
            searchResults.moduleList = search.moduleList;
            if ( ! searchResults.modulesAreIndexed()) {
                search.visible = false;
                indexQuestion.visible = true;
                return;
            }
            openSearchResults();
        }

        function openSearchResults() {
            searchResults.searchText = search.searchText;
            searchResults.findChoice = search.findChoice;
            searchResults.moduleList = search.moduleList;
            search.visible = false;
            searchResults.performSearch();
            searchResults.visible = true;
        }
    }

    SearchResults {
        id: searchResults
        z:2

        onVisibleChanged: {
            if ( ! visible) {
                search.visible = true;
            }
        }
        onIndexingFinishedChanged: {
            indexProgress.visible = false;
            if ( ! searchResults.indexingWasCancelled()) {
                search.openSearchResults();
            }
        }
    }

    Question {
        id: indexQuestion
        background: btStyle.toolbarColor
        text: QT_TR_NOOP("Some of the modules you want to search need to be indexed. Do you want to index them now?")
        onFinished: {
            indexQuestion.visible = false;

            if (answer == true) {
                indexProgress.visible = true;
            } else {
                search.visible = false;
            }
        }
    }

    Progress {
        id: indexProgress

        objectName: "indexProgress"
        value: 0
        minimumValue: 0
        maximumValue: 100
        width:parent.width * 0.85
        height: btStyle.pixelsPerMillimeterY * 30
        anchors.centerIn: parent
        anchors.top: parent.top
        visible: false
        onVisibleChanged: {
            if (visible == true) {
                searchResults.indexModules();
            }
        }
        onCancel: {
            searchResults.cancel();
        }
    }

    ListModel {
        id:searchModel

        function appendModuleChoices(choices) {
            searchModel.clear();
            var firstChoice = "";
            for (var j=0; j<choices.length; ++j) {
                var choice = choices[j];
                if (j>0)
                    firstChoice += ", ";
                firstChoice += choice;
            }
            searchModel.append({"text": firstChoice , "value": firstChoice})

            for (var j=0; j<choices.length; ++j) {
                var choice = choices[j];
                searchModel.append({"text": choice , "value": choice})
            }
        }
        ListElement { text: "ESV"; value: "ESV" }
    }

    InstallManager {
        id: installManager
    }

    InstallManagerChooser {
        id: installManagerChooser

        objectName: "installManagerChooser"
        width: parent.width;
        height: parent.height
        anchors.centerIn: parent
        anchors.top: parent.top
        visible: false
    }

    Progress {
        id: progress

        objectName: "progress"
        value: 0.25
        minimumValue: 0
        maximumValue: 1
        width:parent.width * 0.85
        height: btStyle.pixelsPerMillimeterY * 30
        anchors.centerIn: parent
        anchors.top: parent.top
        visible: false
    }

    ListModel {
        id: mainMenusModel

        ListElement { title: QT_TR_NOOP("New Window");                action: "newWindow" }
        ListElement { title: QT_TR_NOOP("View Window");               action: "view window" }
        ListElement { title: QT_TR_NOOP("Close Window");              action: "close window" }
        ListElement { title: QT_TR_NOOP("Manage Installed Documents");action: "install" }
        ListElement { title: QT_TR_NOOP("Text Font");                 action: "textFontSize" }
        ListElement { title: QT_TR_NOOP("User Interface Font Size");  action: "uiFontSize" }
        ListElement { title: QT_TR_NOOP("Window Arrangement");        action: "windowArrangement" }
        ListElement { title: QT_TR_NOOP("Color Theme");               action: "colortheme" }
        ListElement { title: QT_TR_NOOP("About");                     action: "about" }
    }

    Menus {
        id: mainMenus

        Component.onCompleted: menuSelected.connect(mainMenus.doAction)

        function doAction(action) {
            mainMenus.visible = false;
            if (action == "newWindow") {
                windowManager.newWindow();
            }
            else if (action == "view window") {
                windowManager.createWindowMenus(viewWindowsModel);
                viewWindowsMenus.visible = true;
            }
            else if (action == "close window") {
                windowManager.createWindowMenus(closeWindowsModel);
                closeWindowsMenus.visible = true;
            }
            else if (action == "colortheme") {
                colorThemeMenus.visible = true;
            }
            else if (action == "install") {
                installModules();
            }
            else if (action == "about") {
                aboutDialog.visible = true;
            }
            else if (action == "settings") {
                settings.visible = true;
            }
            else if (action == "textFontSize") {
                setFontDialog.open();
            }
            else if (action == "uiFontSize") {
                uiFontPointSize.visible = true;
            }
            else if (action == "windowArrangement") {
                windowArrangementMenus.visible = true;
            }

        }

        model: mainMenusModel
        topMenuMargin: 100
    }

    ListModel {
        id: windowArrangementModel

        ListElement { title: QT_TR_NOOP("Single");                  action: "single" }
        ListElement { title: QT_TR_NOOP("Tabbed");                  action: "tabbed" }
        ListElement { title: QT_TR_NOOP("Auto-tile");               action: "autoTile" }
        ListElement { title: QT_TR_NOOP("Auto-tile horizontally");  action: "autoTileHor" }
        ListElement { title: QT_TR_NOOP("Auto-tile vertically");    action: "autoTileVer" }
    }

    Menus {
        id: windowArrangementMenus

        Component.onCompleted: menuSelected.connect(windowArrangementMenus.doAction)

        function doAction(action) {
            windowArrangementMenus.visible = false;
            if (action == "single") {
                windowManager.setWindowArrangement(windowManager.single);
            }
            else if (action == "tabbed") {
                windowManager.setWindowArrangement(windowManager.tabLayout);
            }
            else if (action == "autoTile") {
                windowManager.setWindowArrangement(windowManager.autoTile);
            }
            else if (action == "autoTileHor") {
                windowManager.setWindowArrangement(windowManager.autoTileHor);
            }
            else if (action == "autoTileVer") {
                windowManager.setWindowArrangement(windowManager.autoTileVer);
            }
        }

        model: windowArrangementModel
    }

    ListModel {
        id: colorThemeModel

        ListElement { title: QT_TR_NOOP("Dark");                  action: "dark" }
        ListElement { title: QT_TR_NOOP("Light Blue");            action: "lightblue" }
    }

    Menus {
        id: colorThemeMenus

        Component.onCompleted: menuSelected.connect(colorThemeMenus.doAction)

        function doAction(action) {
            colorThemeMenus.visible = false;
            if (action == "dark") {
                btStyle.setStyle(1)
            }
            else if (action == "lightblue") {
                btStyle.setStyle(2)
            }
        }

        model: colorThemeModel
    }

    Menus {
        id: viewWindowsMenus

        model: viewWindowsModel
        visible: false
        Component.onCompleted: menuSelected.connect(viewWindowsMenus.doAction)

        function doAction(action) {
            viewWindowsMenus.visible = false;
            var index = Number(action)
            windowManager.setCurrentTabbedWindow(index);
        }
    }

    Menus {
        id: closeWindowsMenus

        model: closeWindowsModel
        visible: false
        Component.onCompleted: menuSelected.connect(closeWindowsMenus.doAction)

        function doAction(action) {
            closeWindowsMenus.visible = false;
            var index = Number(action)
            windowManager.closeWindow(index);
        }
    }

    FontSizeSlider {
        id: uiFontPointSize

        visible: false
        title: QT_TR_NOOP("User Interface Font Size")

        onVisibleChanged: {
            mainToolbar.enabled = ! uiFontPointSize.visible
            if (visible)
            {
                uiFontPointSize.current = btStyle.uiFontPointSize;
                uiFontPointSize.previous = btStyle.uiFontPointSize;
            }
        }

        onAccepted: {
            btStyle.uiFontPointSize = pointSize
        }
    }

    Question {
        id: quitQuestion
        background: btStyle.toolbarColor
        text: QT_TR_NOOP("Are you sure you want to quit?")
        onFinished: {
            if (answer == true)
                Qt.quit();
        }
    }

    About {
        id: aboutDialog
        visible: false
    }

    SetFont {
        id:setFontDialog

        visible: false
        onVisibleChanged: {
            mainToolbar.enabled = ! setFontDialog.visible
        }

        function open() {
            var index = windowManager.getTopWindowIndex();
            language = windowManager.getLanguageForWindow(index);
            setFontDialog.visible = true;
        }
    }
}
