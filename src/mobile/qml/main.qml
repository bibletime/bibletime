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

import QtQuick.Controls 1.4
import QtQuick 2.2
import QtQml 2.2
import QtQml.Models 2.2
import QtQuick.Window 2.1
import BibleTime 1.0

Window {
    id: root

    property int opacitypopup: 0
    property QtObject component: null;
    property Item window: null;

    function installModules() {
        installManagerChooser.open();
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
    function saveSession() {
        sessionInterface.saveDefaultSession();
    }

    color: "#404040"
    height: btStyle.height
    visible: true
    width:  btStyle.width

    onWidthChanged: btStyle.width = width;
    onHeightChanged: btStyle.height = height;

    Component.onCompleted: {
        setFontDialog.textFontChanged.connect(windowManager.updateTextFont)
        setFontDialog.textFontChanged.connect(magView.updateTextFont)
        sessionInterface.loadDefaultSession();
        if (installInterface.installedModulesCount() == 0)
            installManagerStartup.visible = true;
        else
            informationDialog.openAtStartup();
    }

    Item {
        id: keyReceiver

        objectName: "keyReceiver"
        focus: true
        Keys.forwardTo: [
            searchResultsMenu,
            windowArrangementMenus,
            viewWindowsMenus,
            windowMenus,
            mainMenus,
            settingsMenus,
            bookmarkManagerMenus,
            colorThemeMenus,
            informationDialog,
            gridChooser,
            moduleChooser,
            magView,
            searchResults,
            search,
            textEditor,
            defaultDoc,
            installManagerChooser,
            keyNameChooser,
            treeChooser,
            aboutDialog,
            uiFontPointSize,
            setFontDialog,
            copyVerses,
            bookmarkFoldersParent,
            bookmarkFolders,
            bookmarkManager,
            addBookmark,
            addFolder
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
    }

    About {
        id: aboutDialog

        visible: false
        z: 1
    }

    AddBookmark {
        id: addBookmark

        visible: false
        folderName: bookmarkFolders.currentFolderName
        z: 4
        onBookmarkFolders: {
            bookmarkFolders.visible = true;
        }
        onAddTheBookmark: {
            bookmarkFolders.addTheReference(addBookmark.reference, addBookmark.moduleName);
        }
    }

    AddFolder {
        id: addFolder

        z: 4
        visible: false
        parentFolderName: bookmarkFoldersParent.currentFolderName
        onShowFolders: {
            bookmarkFoldersParent.visible = true;
        }
        onAddFolder: {
            bookmarkFoldersParent.addFolder(folderName);
        }
        onFolderWasAdded: {
            bookmarkFolders.expandAll();
        }
    }

    BookmarkFolders {
        id: bookmarkFolders
        visible: false
        allowNewFolders: true
        z:5
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

    BookmarkFolders {
        id: bookmarkFoldersParent

        visible: false
        allowNewFolders: false
        z:2
        Keys.onReleased: {
            if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && bookmarkFoldersParent.visible == true) {
                bookmarkFoldersParent.visible = false;
                event.accepted = true;
            }
        }
    }

    BookmarkManager {
        id: bookmarkManager

        visible: false
        z: 3
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

        model: bookmarkManager.contextMenuModel
        z:4
        Keys.onReleased: {
            if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape)  && bookmarkManagerMenus.visible == true) {
                event.accepted = true;
                bookmarkManagerMenus.visible = false;
            }
        }

        Component.onCompleted: menuSelected.connect(bookmarkManagerMenus.doAction)
    }

    BtStyle {
        id: btStyle
    }

    ChooseReference {
        id: chooseReference
    }

    Menus {
        id: colorThemeMenus

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
        z: 1
        Keys.onReleased: {
            if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape)  && colorThemeMenus.visible == true) {
                event.accepted = true;
                colorThemeMenus.visible = false;
            }
        }


        Component.onCompleted: menuSelected.connect(colorThemeMenus.doAction)
    }

    ListModel {
        id: colorThemeModel

        ListElement { title: QT_TR_NOOP("Dark");                  action: "dark" }
        ListElement { title: QT_TR_NOOP("Light Blue");            action: "lightblue" }
        ListElement { title: QT_TR_NOOP("Crimson");               action: "crimson" }
    }

    ConfigInterface {
        id: configInterface
    }

    CopyVerses {
        id: copyVerses

        function open() {
            var moduleNames = windowMenus.theWindow.getModuleNames();
            moduleName = moduleNames[0];
            theWindow = windowMenus.theWindow;
            copyVerses.visible = true;
        }

        visible: false
        z: 2
        onVisibleChanged: {
            mainToolbar.enabled = ! copyVerses.visible
            windowManager.toolbarsEnabled = ! copyVerses.visible
        }

    }

    DefaultDoc {
        id: defaultDoc
        visible: false
        z: 1
    }

    GridChooser {
        id: gridChooser

        width: parent.width
        height: parent.height
        visible: false
        z: 2
    }

    Question {
        id: indexQuestion
        background: btStyle.toolbarColor
        text: qsTr("Some of the modules you want to search need to be indexed. Do you want to index them now?")
        z: 4
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

        value: 0
        text: ""
        minimumValue: 0
        maximumValue: 100
        width:parent.width * 0.85
        height: btStyle.pixelsPerMillimeterY * 30
        anchors.centerIn: parent
        anchors.top: parent.top
        visible: false
        z: 4
        onVisibleChanged: {
            if (visible == true) {
                searchResults.indexModules();
            }
        }
        onCancel: {
            searchResults.cancel();
        }
    }

    InformationDialog {
        id: informationDialog

        property string configKey: "GUI/showNewFeatureAtStartup"

        function openAtStartup() {
            var value = configInterface.boolValue(configKey, true);
            if (value)
                open();
        }

        function open() {
            informationDialog.visible = true;
        }

        visible: false
        anchors.centerIn: parent
        height: parent.height/1.4
        width: parent.width * 0.9
        text: {
            var t = "<center><b>";
            t += qsTr("New Feature");
            t += "</b></center><br>"
            t += qsTr("You can write your own comments about Bible verses.");
            t += " " + qsTr("Install the Personal commentary from Crosswire.");
            t += " " + qsTr("Then open the Personal commentary and select a verse.");
            t += " " + qsTr("You can then enter your text.");
            t += "<br><br>";
            t += qsTr("The Personal Commentary can be one of your Parallel Documents.");
            return t;
        }
        onVisibleChanged: {
            mainToolbar.enabled = ! informationDialog.visible
            windowManager.toolbarsEnabled = ! informationDialog.visible
            if (!informationDialog.visible)
                configInterface.setBoolValue(configKey, false);
        }
        z: 3
    }

    InstallInterface {
        id: installInterface

        onUpdateCurrentViews: {
            var sIndex = installInterface.searchSource(source);
            installManagerChooser.sourceIndex = sIndex;
            var cIndex = installInterface.searchCategory(category);
            installManagerChooser.categoryIndex = cIndex;
            var lIndex = installInterface.searchLanguage(language);
            installManagerChooser.languageIndex = lIndex;
        }
    }

    InstallManagerChooser {
        id: installManagerChooser

        width: parent.width;
        height: parent.height
        anchors.centerIn: parent
        anchors.top: parent.top
        visible: false
        z: 1
        onCanceled: installManagerChooser.visible = false;
    }

    Question {
        id: installManagerStartup

        background: btStyle.toolbarColor
        text: qsTr("BibleTime views documents such as Bibles and commentaries. These documents are downloaded and stored locally." +
                   "There are currently no documents. Do you want to download documents now?")
        visible: false
        z: 4
        onFinished: {
            installManagerStartup.visible = false;
            if (answer == true) {
                basicWorks.visible = true;
            }
        }
    }

    Question {
        id: basicWorks

        background: btStyle.toolbarColor
        text: qsTr("Suggested documents are the ESV Bible, the KJV Bible, the StrongsGreek lexicon, and the StrongsHebrew lexicon." +
              "Would you like to automatically download these documents?")
        visible: false
        z: 4
        onFinished: {
            basicWorks.visible = false;
            if (answer == true) {
                installAutomatic.visible = true;
            } else {
                continueDialog.visible = true;
            }
        }
    }

    ContinueDialog {
        id: continueDialog

        background: btStyle.toolbarColor
        text: qsTr("The \"Manage Installed Documents\" window will now be opened. You can open it later from the menus at the upper right of the Main view.")
        visible: false
        z: 3
        onFinished: {
            continueDialog.visible = false;
            installManagerChooser.refreshOnOpen = true;
            installModules();
        }
    }

    InstallAutomatic {
        id: installAutomatic

        visible: false
        z: 1
        onFinished: {
            windowManager.newWindowWithReference("ESV2011", "John 1:1");
        }
    }

    Progress {
        id: installProgress

        value: installInterface.progressValue
        minimumValue: installInterface.progressMin
        maximumValue: installInterface.progressMax
        width:parent.width * 0.85
        height: btStyle.pixelsPerMillimeterY * 30
        anchors.centerIn: parent
        anchors.top: parent.top
        text: installInterface.progressText
        visible: installInterface.progressVisible
        onCancel: installManagerChooser.cancel();
        z: 1
    }

    KeyNameChooser {
        id: keyNameChooser

        width: parent.width
        height: parent.height
        anchors.top: parent.top
        z: 3
        visible: false
    }

    ListModel {
        id: mainMenusModel

        ListElement { title: QT_TR_NOOP("New Window");                action: "newWindow" }
        ListElement { title: QT_TR_NOOP("View Window");               action: "view window" }
        ListElement { title: QT_TR_NOOP("Manage Installed Documents");action: "install" }
        ListElement { title: QT_TR_NOOP("Settings");                  action: "settings" }
        ListElement { title: QT_TR_NOOP("New Features");              action: "new features" }
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
            else if (action == "new features") {
                informationDialog.open();
            }
            else if (action == "settings") {
                settingsMenus.visible = true;
            }
        }

        model: mainMenusModel
        topMenuMargin: 100
        z: 5

        Keys.onReleased: {
            if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape)  && mainMenus.visible == true) {
                event.accepted = true;
                mainMenus.visible = false;
            }
        }

    }

    ModuleChooser {
        id: moduleChooser

        visible: false
        width: Math.min(parent.height, parent.width);
        height: parent.height
        anchors.centerIn: parent
        z: 2
        Keys.onReleased: {
            if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape)  && moduleChooser.visible == true) {
                event.accepted = true;
                moduleChooser.visible = false;
            }
        }
        onCanceled: moduleChooser.visible = false;
    }

    Question {
        id: quitQuestion
        background: btStyle.toolbarColor
        text: qsTranslate("Quit", "Are you sure you want to quit?")
        z: 2
        onFinished: {
            if (answer == true)
                Qt.quit();
        }
    }

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

            objectName: "mainScreen"
            width: screenView.width
            height: screenView.height
            Keys.forwardTo: [keyReceiver]
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

            SessionInterface {
                id: sessionInterface

                function loadDefaultSession() {
                    var windowList = getWindowList();
                    var count = windowList.length
                    if (installInterface.installedModulesCount() > 0) {
                        for (var i=0; i < count; ++i) {
                            var window = windowList[i];
                            var modules = getWindowModuleList(window);
                            var key = getWindowKey(window);
                            windowManager.openWindow("", modules, key);
                        }
                    }
                    var color = getColorTheme();
                    btStyle.setStyle(color);
                    var winMode = getWindowArrangementMode();
                    windowManager.windowArrangement = winMode;
                }

                function saveDefaultSession() {
                    var color = btStyle.getStyle();
                    setColorTheme(color);
                    var winMode = windowManager.windowArrangement;
                    setWindowArrangementMode(winMode);
                    var windowList = [];
                    var count = windowManager.getWindowCount();
                    for (var i=0; i<count; ++i) {
                        var win = i.toString();
                        windowList[i] = win;
                        var window = windowManager.getWindow(i);
                        var moduleList = window.getModuleNames();
                        sessionInterface.setWindowModuleList(i, moduleList);
                        var key = window.getReference();
                        sessionInterface.setWindowKey(i, key);
                    }
                    sessionInterface.setWindowList(windowList);
                }
            }

            WindowManager {
                id: windowManager

                anchors.top: spacer.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                color: btStyle.textBackgroundColor
                visible: true
                onWindowMenus: {
                    windowMenus.theWindow = window
                    window.createMenus(windowMenusModel)
                    windowMenus.visible = true;
                }
            }
        }

        SearchResults {
            id: searchResults

            z:2
            width: screenView.width
            height: screenView.height
            onResultsMenuRequested: {
                searchResultsMenu.visible = true;
            }
            onResultsFinished: {
                screenView.changeScreen(screenModel.main);
                searchResultsMenu.visible = false;
            }
            onIndexingFinishedChanged: {
                indexProgress.visible = false;
                if ( ! searchResults.indexingWasCancelled()) {
                    search.openSearchResults();
                }
            }
            onProgressTextChanged: {
                indexProgress.text = text;
            }
            onProgressValueChanged: {
                indexProgress.value = value;
            }
        }

        Search {
            id: search

            function openSearchResults() {
                searchResults.searchText = search.searchText;
                searchResults.findChoice = search.findChoice;
                searchResults.moduleList = search.moduleList;
                searchResults.performSearch();
                screenView.changeScreen(screenModel.results);
            }

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
            if (currentIndex == screenModel.search) {
                var moduleNames = windowManager.getUniqueModuleNames();
                searchModel.appendModuleChoices(moduleNames);
            }
        }

        NumberAnimation on contentX {
            id: screenAnimation

            to: screenView.width * screenView.nextIndex
            duration: 400
            running: false
        }
    }


    ListModel {
        id: searchModel

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

    Menus {
        id: searchResultsMenu

        function doAction(action) {
            searchResultsMenu.visible = false;
            var module = searchResults.getModule();
            var reference = searchResults.getReference();
            if (action == "newWindow") {
                screenView.changeScreen(screenModel.main);
                windowManager.newWindowWithReference(module, reference);
            } else if (action == "viewReferences") {
                viewReferencesScreen(module, reference);
            }
        }

        model: searchResultsMenuModel
        z: 2
        Keys.onReleased: {
            if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape)  && searchResultsMenu.visible == true) {
                event.accepted = true;
                searchResultsMenu.visible = false;
            }
        }

        Component.onCompleted: menuSelected.connect(searchResultsMenu.doAction)
    }

    ListModel {
        id: searchResultsMenuModel

        ListElement { title: QT_TR_NOOP("New Window");                action: "newWindow" }
        ListElement { title: QT_TR_NOOP("View References");           action: "viewReferences" }
    }

    SetFont {
        id: setFontDialog

        function open() {
            var index = windowManager.getTopWindowIndex();
            language = windowManager.getLanguageForWindow(index);
            setFontDialog.visible = true;
        }

        visible: false
        onVisibleChanged: {
            mainToolbar.enabled = ! setFontDialog.visible
            windowManager.toolbarsEnabled = ! setFontDialog.visible
        }
    }

    Menus {
        id: settingsMenus

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
            else if (action == "defaultDoc") {
                defaultDoc.visible = true;
            }
        }

        model: settingsModel
        Keys.onReleased: {
            if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape)  && settingsMenus.visible == true) {
                event.accepted = true;
                settingsMenus.visible = false;
            }
        }


        Component.onCompleted: menuSelected.connect(settingsMenus.doAction)
    }

    ListModel {
        id: settingsModel

        ListElement { title: QT_TR_NOOP("Text Font");                 action: "textFontSize" }
        ListElement { title: QT_TR_NOOP("User Interface Font Size");  action: "uiFontSize" }
        ListElement { title: QT_TR_NOOP("Window Arrangement");        action: "windowArrangement" }
        ListElement { title: QT_TR_NOOP("Color Theme");               action: "colortheme" }
        ListElement { title: QT_TR_NOOP("Default Documents");         action: "defaultDoc" }
    }

    TextEditor {
        id: textEditor

        anchors.left: root.left
        anchors.top: root.top
        height: root.height
        width: root.width
        visible: false
        Keys.onReleased: {
            if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape)  && textEditor.visible == true) {
                event.accepted = true;
                textEditor.visible = false;
            }
        }
        onVisibleChanged: {
            if (!visible)
                keyReceiver.forceActiveFocus();
        }
    }

    TreeChooser {
        id: treeChooser

        width:parent.width
        height: parent.height
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        path: ""
        visible: false
        z: 100
        onNext: {
            chooseReference.next(childText);
        }
        onBack: {
            chooseReference.back();
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

    VerseChooser {
        id: verseChooser
    }

    Menus {
        id: viewWindowsMenus

        function doAction(action) {
            viewWindowsMenus.visible = false;
            var index = Number(action)
            windowManager.setCurrentTabbedWindow(index);
        }

        model: viewWindowsModel
        visible: false
        Keys.onReleased: {
            if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape)  && viewWindowsMenus.visible == true) {
                event.accepted = true;
                viewWindowsMenus.visible = false;
            }
        }

        Component.onCompleted: menuSelected.connect(viewWindowsMenus.doAction)
    }

    ListModel {
        id: viewWindowsModel

        ListElement { title: ""; action: "" }
    }

    Menus {
        id: windowArrangementMenus

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
        Keys.onReleased: {
            if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape)  && windowArrangementMenus.visible == true) {
                event.accepted = true;
                windowArrangementMenus.visible = false;
            }
        }


        Component.onCompleted: menuSelected.connect(windowArrangementMenus.doAction)
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
        id: windowMenus

        property variant theWindow

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
            else if (action == "copy") {
                copyVerses.open();
            }
            else if (action == "addParallel") {
                theWindow.addParallelModule();
            }
            else if (action == "removeParallel") {
                theWindow.removeParallelModule();
            }
            else if (action == "debugData") {
                theWindow.debugData();
            }
        }

        model: windowMenusModel
        z:2
        Keys.onReleased: {
            if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape)  && windowMenus.visible == true) {
                event.accepted = true;
                windowMenus.visible = false;
            }
        }

        Component.onCompleted: menuSelected.connect(windowMenus.doAction)

    }

    ListModel {
        id: windowMenusModel

        ListElement { title: ""; action: "" }
    }
}
