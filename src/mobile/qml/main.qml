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
import QtQml 2.2
import QtQml.Models 2.2
import QtQuick.Window 2.1
import BibleTime 1.0

Rectangle {
    id: root

    color: "#404040"
    property int opacitypopup: 0
    property QtObject component: null;
    property Item window: null;

    Component.onCompleted: {
        setFontDialog.textFontChanged.connect(windowManager.updateTextFont)
        setFontDialog.textFontChanged.connect(magView.updateTextFont)
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
        screenView.changeScreen(screenModel.search);
    }

    function viewReferencesScreen(moduleName, reference) {
        magView.initialize();
        magView.setModule(moduleName);
        magView.setReference(reference);
        magView.scrollDocumentViewToCurrentReference();
        screenView.changeScreen(screenModel.references);
    }

    Keys.forwardTo: [
        gridChooser,
        moduleChooser,
        mainMenus,
        windowArrangementMenus,
        viewWindowsMenus,
        windowMenus,
        magView,
        searchResults,
        search,
        installManagerChooser,
        keyNameChooser,
        treeChooser,
        aboutDialog,
        uiFontPointSize,
        setFontDialog,
        parentBookmarkFolders,
        bookmarkFolders,
        bookmarkManager,
        addBookmark,
        addFolder,
        bookmarkManagerMenus,
        colorThemeMenus
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

    width:  500
    height: 750

    rotation: 0

    ObjectModel {
        id: screenModel

        property int references: 0
        property int main: 1
        property int results: 2
        property int search: 3

        MagView {
            id: magView

            width: screenView.width
            height: screenView.height

            onMagFinished: {
                screenView.changeScreen(screenModel.main);
            }
        }

        Rectangle {
            id: mainScreen

            width: screenView.width
            height: screenView.height

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
                color: btStyle.textBackgroundColor
                visible: false

                onWindowMenus: {
                    windowMenus.theWindow = window
                    windowMenus.visible = true;
                }
            }
        }

        SearchResults {
            id: searchResults

            z:2
            width: screenView.width
            height: screenView.height

            onResultsFinished: {
                screenView.changeScreen(screenModel.main);
            }
            onIndexingFinishedChanged: {
                indexProgress.visible = false;
                if ( ! searchResults.indexingWasCancelled()) {
                    search.openSearchResults();
                }
            }
        }

        Search {
            id: search

            width: screenView.width
            height: screenView.height
            moduleChoices: searchModel
            onSearchRequest: {
                searchResults.moduleList = search.moduleList;
                if ( ! searchResults.modulesAreIndexed()) {
                    indexQuestion.visible = true;
                    return;
                }
                openSearchResults();
            }
            onSearchFinished: {
                screenView.changeScreen(screenModel.main);
            }

            function openSearchResults() {
                searchResults.searchText = search.searchText;
                searchResults.findChoice = search.findChoice;
                searchResults.moduleList = search.moduleList;
                searchResults.performSearch();
                screenView.changeScreen(screenModel.results);
            }
        }
    }

    ListView {
        id: screenView

        property int nextIndex: 0

        function changeScreen(screen) {
            screenView.nextIndex = screen
            screenAnimation.start();
        }

        anchors.fill: parent
        model: screenModel
        preferredHighlightBegin: 0; preferredHighlightEnd: 0
        highlightRangeMode: ListView.StrictlyEnforceRange
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem; flickDeceleration: 2000
        highlightFollowsCurrentItem: true
        currentIndex: screenModel.main
        maximumFlickVelocity: 4000
        highlightMoveDuration: 4000

        onCurrentIndexChanged: {
            magView.initialize();
        }

        NumberAnimation on contentX {
            id: screenAnimation

            to: screenView.width * screenView.nextIndex
            duration: 400
            running: false
        }
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

    Question {
        id: startupBookshelfManager

        objectName: "startupBookshelfManager"
        background: btStyle.toolbarColor
        text: qsTr("BibleTime views documents such as Bibles and commentaries. These documents are downloaded and stored locally." +
                   "There are currently no documents. Do you want to install documents now?")
        onFinished: {
            startupBookshelfManager.visible = false;
            if (answer == true) {
                installManagerChooser.refreshOnOpen = true;
                installModules();
            }
        }
    }


    Question {
        id: indexQuestion
        background: btStyle.toolbarColor
        text: qsTr("Some of the modules you want to search need to be indexed. Do you want to index them now?")
        onFinished: {
            indexQuestion.visible = false;

            if (answer == true) {
                indexProgress.visible = true;
            } else {
                screenView.changeScreen(screenModel.main);
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
        ListElement { title: QT_TR_NOOP("Manage Installed Documents");action: "install" }
        ListElement { title: QT_TR_NOOP("Settings");                  action: "settings" }
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
            else if (action == "install") {
                installModules();
            }
            else if (action == "about") {
                aboutDialog.visible = true;
            }
            else if (action == "settings") {
                settingsMenus.visible = true;
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
        ListElement { title: QT_TR_NOOP("Crimson");               action: "crimson" }
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
            else if (action == "crimson") {
                btStyle.setStyle(3)
            }
        }

        model: colorThemeModel
    }

    ListModel {
        id: settingsModel

        ListElement { title: QT_TR_NOOP("Text Font");                 action: "textFontSize" }
        ListElement { title: QT_TR_NOOP("User Interface Font Size");  action: "uiFontSize" }
        ListElement { title: QT_TR_NOOP("Window Arrangement");        action: "windowArrangement" }
        ListElement { title: QT_TR_NOOP("Color Theme");               action: "colortheme" }
    }

    Menus {
        id: settingsMenus

        Component.onCompleted: menuSelected.connect(settingsMenus.doAction)

        function doAction(action) {
            settingsMenus.visible = false;
            if (action == "colortheme") {
                colorThemeMenus.visible = true;
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

        model: settingsModel
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
        text: qsTranslate("Quit", "Are you sure you want to quit?")
        onFinished: {
            if (answer == true)
                Qt.quit();
        }
    }

    About {
        id: aboutDialog
        visible: false
    }

    ListModel {
        id: windowMenusModel

        ListElement { title: QT_TR_NOOP("Add BookMark"); action: "addBookmark" }
        ListElement { title: QT_TR_NOOP("Bookmarks");    action: "bookmarks" }
        ListElement { title: QT_TR_NOOP("View References");    action: "viewReferences" }
        ListElement { title: QT_TR_NOOP("Close Window");    action: "close window" }
    }

    Menus {
        id: windowMenus

        property variant theWindow

        model: windowMenusModel
        z:10

        Component.onCompleted: menuSelected.connect(windowMenus.doAction)

        function doAction(action) {
            windowMenus.visible = false;
            if (action == "addBookmark") {
                var moduleName = theWindow.getModule();
                var reference = theWindow.getReference();
                addBookmark.reference = reference
                addBookmark.moduleName = moduleName
                addBookmark.visible = true;
            }
            else if (action == "bookmarks") {
                bookmarkManager.visible = true;
            }
            else if (action == "viewReferences") {
                var moduleName = theWindow.getModule();
                var reference = theWindow.getReference();
                viewReferencesScreen(moduleName, reference)
            }
            else if (action == "close window") {
                var index = windowManager.findIndexOfWindow(theWindow);
                windowManager.closeWindow(index);
            }
        }
    }


    AddBookmark {
        id: addBookmark

        visible: false
        folderName: bookmarkFolders.currentFolderName

        onBookmarkFolders: {
            bookmarkFolders.visible = true;
        }

        onAddTheBookmark: {
            bookmarkFolders.addTheReference(addBookmark.reference, addBookmark.moduleName);
        }
    }

    BookmarkFolders {
        id: bookmarkFolders
        visible: false
        allowNewFolders: true
        z:100

        onNewFolder: {
            addFolder.visible = true;
        }

        Keys.onReleased: {
            if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && bookmarkFolders.visible == true) {
                bookmarkFolders.visible = false;
                event.accepted = true;
            }
        }
    }

    AddFolder {
        id: addFolder

        z: 101
        visible: false
        parentFolderName: parentBookmarkFolders.currentFolderName

        onShowFolders: {
            parentBookmarkFolders.visible = true;
        }

        onAddFolder: {
            parentBookmarkFolders.addFolder(folderName);
        }

        onFolderWasAdded: {
            bookmarkFolders.expandAll();
        }
    }

    BookmarkFolders {
        id: parentBookmarkFolders
        visible: false
        allowNewFolders: false
        z:102

        Keys.onReleased: {
            if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && parentBookmarkFolders.visible == true) {
                parentBookmarkFolders.visible = false;
                event.accepted = true;
            }
        }
    }

    BookmarkManager {
        id: bookmarkManager

        visible: false

        Keys.onReleased: {
            if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && bookmarkManager.visible == true) {
                bookmarkManager.visible = false;
                event.accepted = true;
            }
        }

        onBookmarkItemClicked: {
            bookmarkManagerMenus.visible = true;
        }

        onOpenReference: {
            windowMenus.theWindow.setModule(module);
            windowMenus.theWindow.setKey(reference);
            windowMenus.theWindow.setHistoryPoint();
            bookmarkManager.visible = false;
        }
    }


    Menus {
        id: bookmarkManagerMenus

        property variant theWindow

        model: bookmarkManager.contextMenuModel
        z:10

        Component.onCompleted: menuSelected.connect(bookmarkManagerMenus.doAction)

        function doAction(action) {
            bookmarkManagerMenus.visible = false;
            if (action == "open") {
                windowMenus.theWindow.setModule(bookmarkManager.module);
                windowMenus.theWindow.setKey(bookmarkManager.reference);
                bookmarkManager.visible = false;
                return;
            }
            bookmarkManager.doContextMenu(action);
        }
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
