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
    id: windowManager

    objectName: "WindowManager"

    property var windows: []

    property int autoTileVer: 1
    property int autoTileHor: 2
    property int single:      3  // Tile in Desktop
    //                        4 is Manual in Desktop
    property int autoTile:    5
    property int tabLayout:   6

    property int windowArrangement: tabLayout
    property bool toolbarsEnabled: true

    signal setFontForLanguage(string language)
    signal windowMenus(variant window)

    onWindowArrangementChanged: layoutWindows()
    onToolbarsEnabledChanged: {
        for (var i=0; i<windows.length; ++i) {
            var window = windows[i];
            window.toolbarEnabled = windowManager.toolbarsEnabled
        }
    }

    function getTopWindowIndex() {
        if (windowArrangement == single || windowArrangement == tabLayout)
            return tabbedWindows.current;
        return 0;
    }

    function getLanguageForWindow(index) {
        if (index < 0 || index >= windows.length)
            return "";
        return windows[index].getModuleLanguage();
    }

    function updateTextFont() {
        for (var i=0; i<windows.length; ++i) {
            var window = windows[i];
            var name = window.updateTextFonts();
        }
    }

    function getWindowCount() {
        return windows.length;
    }

    function getWindow(index) {
        return windows[index];
    }

    function saveWindowStateToConfig(windowIndex) {
        var window = windows[windowIndex];
        window.saveWindowStateToConfig(windowIndex);
    }

    function getModuleNames() {
        var names = [];
        for (var i=0; i<windows.length; ++i) {
            var window = windows[i];
            var winNames = window.getModuleNames();
            names.push.apply(names, winNames);
        }
        return names;
    }

    function getUniqueModuleNames() {
        var moduleNames = getModuleNames();
        var uniqueNames = [];
        for (var i=0; i<moduleNames.length; ++i) {
            var name = moduleNames[i];
            var index = uniqueNames.indexOf(name);
            if (index < 0) {
                uniqueNames.push(name);
            }
        }
        return uniqueNames;
    }

    function setCurrentTabbedWindow(index) {
        tabbedWindows.current = index;
    }

    function setNextWindow() {
        var index = tabbedWindows.current + 1;
        if (index >= windows.length)
            index = 0;
        setCurrentTabbedWindow(index);
    }

    function setPreviousWindow() {
        var index = tabbedWindows.current - 1;
        if (index < 0)
            index = windows.length - 1;
        setCurrentTabbedWindow(index);
    }

    function windowMenusSlot(window) {
        windowMenus(window);
    }

    function moduleChooserRequestedSlot(window, moduleNumber) {
        moduleChooser.backText = qsTranslate("Navigation", "Main");
        moduleChooser.visible = true;
    }

    function closeWindow(index) {
        var window = windows[index];
        windows.splice(index,1);
        window.destroy();
        layoutWindows();
    }

    function setWindowArrangement(arrangement) {
        if (arrangement < autoTileVer || arrangement > tabLayout)
            return;
        windowArrangement = arrangement;
    }

    function findIndexOfWindow(window) {
        for (var i=0; i<windows.length; ++i) {
            if (window == windows[i])
                return i;
        }
        return -1;
    }

    function createWindowMenus(model) {
        model.clear();
        for (var i=0; i<windows.length; ++i) {
            var window = windows[i];
            model.append (
                        { title: window.title, action: i.toString() }
                        )
        }
    }

    function newWindowWithReference(moduleName, reference) {
        var modules = [moduleName];
        openWindow("", modules, reference);
    }

    function newWindow() {
        moduleChooser.moduleSelected.connect(openWindowSlot);
        moduleChooser.bibleCommentaryOnly = false;
        moduleChooser.backText = qsTranslate("Navigation", "Main");
        moduleChooser.visible = true;
    }

    function openWindowSlot() {
        moduleChooser.moduleSelected.disconnect(openWindowSlot);
        var modules = [moduleChooser.selectedModule];
        openWindow(moduleChooser.selectedCategory, modules, "")
    }

    function openWindow(category, modules, key) {
        if (category == "" ||
            category == "Bibles" ||
            category == "Cults/Unorthodox" ||
            category == "Commentaries" ||
            category == "Books" ||
            category == "Lexicons and Dictionaries" ||
            category == "Daily Devotionals")
            component = Qt.createComponent("Window.qml");
        else {
            console.log(category, " are not yet supported.");
            return;
        }

        window = component.createObject(null, {"width": 250, "height": 200});

        if (window == null) {
            // Error Handling
            console.log("Error creating object");
        }
        else {
            window.setModules(modules);
            window.setModuleToBeginning();
            window.windowMenusDialog.connect(windowManager, windowMenusSlot);
            window.moduleChooserRequest.connect(windowManager, moduleChooserRequestedSlot);
            windows.push(window)
            layoutWindows();
            var curWindow = windows.length -1;
            selectWindow(curWindow);
            if (key !== "")
                window.setKey(key);
            window.setHistoryPoint();
        }
    }

    function setFont(lang) {
        windowManager.setFontForLanguage(lang)
    }

    function layoutTiles(rows, columns)
    {
        gridWindows.columns = columns;
        gridWindows.rows = rows;
        var width = gridWindows.width/columns -2;
        var height = gridWindows.height/rows -2;

        for (var i=0; i<windows.length; ++i) {
            var window = windows[i];
            window.anchors.fill = undefined
            window.height = height;
            window.width = width;
            window.parent = gridWindows;
            window.border.width = 1;
        }
    }

    function arrangeSingleWindow() {
        tabbedWindows.z = 1;
        tabbedWindows.tabVisible = false;
        for (var i=0; i<windows.length; ++i) {
            var window = windows[i];
            window.parent = tabbedWindowsStack;
            window.anchors.fill = tabbedWindowsStack;
            window.border.width = 0;
        }
    }

    function arrangeTabbedWindows() {
        tabbedWindows.z = 1;
        tabbedWindows.tabVisible = true;
        for (var i=0; i<windows.length; ++i) {
            var window = windows[i];
            window.parent = tabbedWindowsStack;
            window.anchors.fill = tabbedWindowsStack
            window.border.width = 0;
        }
    }

    function arrangeTiledWindows() {
        gridWindows.z = 1;
        var columns = 1;
        var rows = 1;
        var count = windows.length;

        if (windowArrangement == autoTile) {
            if (count > 1) {
                columns = 2
                rows = Math.floor((count+1)/2);
            }
        }
        else if (windowArrangement == autoTileHor)
        {
            rows = count;
        }
        else if (windowArrangement == autoTileVer)
        {
            columns = count;
        }
        layoutTiles(rows, columns);
    }

    function layoutWindows() {

        tabbedWindows.z = -2;
        gridWindows.z = -3;

        if (windowArrangement == single) {
            arrangeSingleWindow();
        }
        else if (windowArrangement == tabLayout) {
            arrangeTabbedWindows();
        }
        else {
            arrangeTiledWindows();
        }
        tipText.visible = false;
        tipVisibleTimer.start();
    }

    function selectWindow(n) {
        if (windowArrangement == tabLayout || windowArrangement == single) {
            tabbedWindows.current = n;
        }
    }

    Timer {
        id: tipVisibleTimer

        interval: 2000
        onTriggered: {
            tipText.visible = (windows.length == 0);
        }
    }

    Text {
        id: tipText

        text: qsTranslate("Welcome", "Use the \"New Window\" menu to open a document.")
        anchors.left: windowManager.left
        anchors.right: windowManager.right
        anchors.verticalCenter: windowManager.verticalCenter
        anchors.margins: btStyle.pixelsPerMillimeterX*6
        horizontalAlignment: Text.AlignJustify
        font.pointSize: btStyle.uiFontPointSize;
        color: btStyle.textColor
        visible: false
        wrapMode: Text.Wrap
        z: 10
    }

    Grid {
        id: gridWindows

        objectName: "gridWindows"
        anchors.fill: parent
        columns: 2
        spacing: 0
        z: 2
        onWidthChanged: {
            layoutWindows();
        }
        onHeightChanged: {
            layoutWindows();
        }
    }

    Rectangle {
        id: tabbedWindows

        default property alias content: tabbedWindowsStack.children
        property bool tabVisible: true
        property int current: 0

        function changeTabs() {
            setOpacities();
        }

        function setOpacities() {
            for (var i = 0; i < tabbedWindowsStack.children.length; ++i) {
                tabbedWindowsStack.children[i].z = (i == current ? 1 : 0)
            }
        }
        color: btStyle.toolbarColor
        objectName: "tabbedWindows"
        anchors.fill: parent
        onCurrentChanged: changeTabs()
        Component.onCompleted: changeTabs()

        Row {
            id: header

            objectName: "header"

            Repeater {
                id: tabRepeater

                function setColors() {
                    if (tabbedWindows.current == tabbedWindowsStack.index) {
                        tabImage.color = btStyle.windowTabSelected
                        tabText.color = btStyle.windowTabTextSelected
                    }
                    else {
                        tabImage.color = btStyle.windowTab
                        tabText.color = btStyle.windowTabText
                    }
                }

                model: tabbedWindowsStack.children.length
                delegate: Rectangle {
                    id: tab

                    function calculateTabWidth() {
                        var tabWidth = (tabbedWindows.width) / tabbedWindowsStack.children.length;
                        tabWidth = Math.min(tabbedWindows.width/2, tabWidth)
                        return tabWidth;
                    }

                    visible: tabbedWindows.tabVisible
                    width: {
                        calculateTabWidth()
                    }
                    height: {
                        var pixel = btStyle.pixelsPerMillimeterY * 6;
                        var uiFont = btStyle.uiFontPointSize * 4.4;
                        var mix = pixel * 0.7 + uiFont * 0.3;
                        return Math.max(pixel, mix);
                    }
                    color: btStyle.toolbarColor

                    Rectangle {
                        id: tabImage

                        anchors { fill: parent; leftMargin: 4; topMargin: 4; rightMargin: 4 }
                        color: {
                            if (tabbedWindows.current == index)
                                return btStyle.windowTabSelected
                            else
                                return btStyle.windowTab
                        }
                        radius: height/2

                        Rectangle {
                            color: tabImage.color
                            height: tabImage.height/2
                            width: tabImage.width
                            anchors.left: tabImage.left
                            anchors.bottom: tabImage.bottom
                        }

                        Text {
                            id: tabText

                            horizontalAlignment: Qt.AlignHCenter;
                            verticalAlignment: Qt.AlignVCenter
                            anchors.fill: parent
                            anchors.topMargin: 4
                            font.pointSize: btStyle.uiFontPointSize -1
                            text: tabbedWindowsStack.children[index].title
                            elide: Text.ElideLeft
                            color: {
                                if (tabbedWindows.current == index)
                                    return btStyle.windowTabTextSelected
                                else
                                    return btStyle.windowTabText
                            }
                        }
                    }

                    MouseArea {
                        id: tabMouseArea

                        anchors.fill: parent
                        onClicked: {
                            tabbedWindows.current = index
                        }
                    }
                }
            }
        }

        Item {
            id: tabbedWindowsStack

            objectName: "tabbedWindowsStack"
            width: parent.width
            anchors.top: header.bottom;
            anchors.bottom: tabbedWindows.bottom
        }
    }

}
