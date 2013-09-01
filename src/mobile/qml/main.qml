import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import QtWebKit 3.0
import BibleTime 1.0

Rectangle {
    id: root

    property int opacitypopup: 0
    property QtObject component: null;
    property Item window: null;
    property var windows: []
    property int single: 0
    property int tabLayout: 1
    property int autoTile: 2
    property int autoTileHor: 3
    property int autoTileVer: 4
    property int windowLayout: single

    ListModel {
        id: windowsModel

        ListElement { title: ""; action: "" }
    }

    function createWindowMenus() {
        windowsModel.clear();
        for (var i=0; i<windows.length; ++i) {
            var window = windows[i];
            console.log(window.title)
            windowsModel.append (
                        { title: window.title, action: i.toString() }
                        )
        }
        windowTitlesMenus.model = windowsModel
        windowTitlesMenus.visible = true;
    }

    function layoutTiles(rows, columns)
    {
        console.log("layoutTitles");
        gridWindows.columns = columns;
        gridWindows.rows = rows;
        var width = gridWindows.width/columns -2;
        var height = gridWindows.height/rows -2;

        for (var i=0; i<windows.length; ++i) {
            console.log("xxxxx");
            var window = windows[i];
            window.anchors.fill = undefined
            window.height = height;
            window.width = width;
            window.parent = gridWindows;
        }
    }

    function arrangeSingleWindow() {
        tabbedWindows.z = 1;
        tabbedWindows.tabVisible = false;
        for (var i=0; i<windows.length; ++i) {
            var window = windows[i];
//            console.log(window, i)
            window.parent = tabbedWindowsStack;
            console.log("w ",window, tabbedWindowsStack)
            window.anchors.fill = tabbedWindowsStack
        }
    }

    function arrangeTabbedWindows() {
        tabbedWindows.z = 1;
        tabbedWindows.tabVisible = true;
        for (var i=0; i<windows.length; ++i) {
            var window = windows[i];
            window.parent = tabbedWindowsStack;
            window.anchors.fill = tabbedWindowsStack
        }
    }

    function arrangeTiledWindows() {
        gridWindows.z = 1;
        var columns = 1;
        var rows = 1;
        var count = windows.length;

        if (windowLayout == autoTile) {
            if (count > 1) {
                columns = 2
                rows = Math.floor((count+1)/2);
            }
        }
        else if (windowLayout == autoTileHor)
        {
            rows = count;
        }
        else if (windowLayout == autoTileVer)
        {
            columns = count;
        }
        layoutTiles(rows, columns);
    }

    function layoutWindows() {
        tabbedWindows.z = -2;
        gridWindows.z = -3;

//        console.log("layoutWindows")
        if (windowLayout == single) {
            arrangeSingleWindow();
        }
        else if (windowLayout == tabLayout) {
            arrangeTabbedWindows();
        }
        else {
            arrangeTiledWindows();
        }
    }


    function selectWindow(n) {
        if (windowLayout == tabLayout || windowLayout == single) {
            tabbedWindows.current = n;
        }
    }

    function newWindow() {
        moduleChooser.moduleSelected.connect(openWindowSlot);
        moduleChooser.visible = true;
    }

    function openWindowSlot() {
        moduleChooser.moduleSelected.disconnect(openWindowSlot);
        openWindow(moduleChooser.selectedCategory, moduleChooser.selectedModule)
    }

    function openWindow(category, module) {
        if (category == "Bibles")
            component = Qt.createComponent("Window.qml");
        else if (category == "Commentaries")
            component = Qt.createComponent("Window.qml");
        else if (category == "Books")
            component = Qt.createComponent("Window.qml");
        else {
            console.log(category, " are not yet supported.");
            return;
        }

        window = component.createObject(null, {"width": 250, "height": 200});
        window.setModule(module);

        if (window == null) {
            // Error Handling
            console.log("Error creating object");
        }
        else {
            windows.push(window)
            layoutWindows();
            var curWindow = windows.length -1;
            selectWindow(curWindow);
        }
    }

    function installModules() {
        installManager.openChooser();
    }

    //    width: 1280 // Nexus 7 (2012)
    //    height: 800

    width:  480   // Phone
    height: 800

    rotation: 0

    MainToolbar {
        id: mainToolbar

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        height: 30
        onButtonClicked: {
            mainMenus.visible = ! mainMenus.visible;
        }
    }
    Rectangle {
        id: spacer

        anchors.top: mainToolbar.bottom
        height:2
        width: parent.width
        color: "#646464"
    }

    Rectangle {
        id: windowArea

        anchors.top: spacer.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "#646464"

        Grid {
            id: gridWindows

            objectName: "gridWindows"
            anchors.fill: parent
            columns: 2
            spacing: 2
            z: 2
        }

        Item {
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
                            return tabWidth;
                        }

                        visible: tabbedWindows.tabVisible
                        //width: (tabbedWindows.width) / tabbedWindowsStack.children.length;
                        width: {
                            calculateTabWidth()
                        }
                        height: 36

                        Rectangle {
                            id: tabBorder
                            width: parent.width; height: 1
                            anchors { bottom: parent.bottom; bottomMargin: 1 }
                            color: "#acb2c2"
                        }

                        Rectangle {
                            id: tabImage

                            anchors { fill: parent; leftMargin: 8; topMargin: 4; rightMargin: 8 }
                            color: {
                                if (tabbedWindows.current == index)
                                    return btStyle.windowTabSelected
                                else
                                    return btStyle.windowTab
                            }
                            border.color: btStyle.windowTab
                            border.width: 2

                            Text {
                                id: tabText

                                horizontalAlignment: Qt.AlignHCenter;
                                verticalAlignment: Qt.AlignVCenter
                                anchors.fill: parent
                                anchors.topMargin: 6
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

        ListModel {
            id: mainMenusModel

            ListElement { title: QT_TR_NOOP("New Window");              action: "newWindow" }
            ListElement { title: QT_TR_NOOP("Bookshelf Manager");       action: "install" }
            ListElement { title: QT_TR_NOOP("Windows");                 action: "windows" }
            ListElement { title: QT_TR_NOOP("Window Arrangement");      action: "windowArrangement" }
            //            ListElement { title: QT_TR_NOOP("Gnome Style");             action: "gnomeStyle" }
            //            ListElement { title: QT_TR_NOOP("Android Style");           action: "androidStyle" }
        }

        Menus {
            id: mainMenus

            Component.onCompleted: menuSelected.connect(mainMenus.doAction)

            function doAction(action) {
                mainMenus.visible = false;
                if (action == "newWindow") {
                    root.newWindow();
                }
                else if (action == "windows") {
                    createWindowMenus();
                }
                else if (action == "windowArrangement") {
                    windowArrangementMenus.visible = true;
                }
                else if (action == "gnomeStyle") {
                    btStyle.setStyle(1)
                }
                else if (action == "androidStyle") {
                    btStyle.setStyle(2)
                }
                else if (action == "install") {
                    installModules();
                }

            }

            model: mainMenusModel
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

            menusWidth: 350

            Component.onCompleted: menuSelected.connect(windowArrangementMenus.doAction)

            function doAction(action) {
                windowArrangementMenus.visible = false;
                if (action == "single") {
                    windowLayout = single
                    layoutWindows();
                }
                else if (action == "tabbed") {
                    windowLayout = tabLayout;
                    layoutWindows();
                }
                else if (action == "autoTile") {
                    windowLayout = autoTile;
                    layoutWindows();
                }
                else if (action == "autoTileHor") {
                    windowLayout = autoTileHor;
                    layoutWindows();
                }
                else if (action == "autoTileVer") {
                    windowLayout = autoTileVer;
                    layoutWindows();
                }
            }

            model: windowArrangementModel
            anchors.top: parent.bottom
            anchors.right: parent.right
        }

        Menus {
            id: windowTitlesMenus

            model: windowsModel
            menusWidth: 350
            anchors.top: parent.bottom
            anchors.right: parent.right
            visible: false
            Component.onCompleted: menuSelected.connect(windowTitlesMenus.doAction)

            function doAction(action) {
                windowTitlesMenus.visible = false;
                var index = Number(action)
                if (index == 0)
                    return;
                var windowList = root.windows;
                var window = windowList.slice(index, index+1);
                windowList.splice(index,1);
                windowList.unshift(window);
                root.windows = windowList;
                layoutWindows();
            }
        }
    }

    GridChooser {
        id: gridChooser

        objectName: "gridChooser"
        width: parent.width
        height: parent.height
        z:-5000
        opacity: 0
    }

    BtStyle {
        id: btStyle
    }

    InstallManager {
        id: installManager
    }

    ModuleChooser {
        id: moduleChooser

        objectName: "moduleChooser"
        visible: false
        width:400
        height: parent.height
        anchors.centerIn: parent
    }

    TreeChooser {
        id: treeChooser

        objectName: "treeChooser"
        width:650
        height: parent.height
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        path: ""
        visible: false
        z: 100
    }

    InstallManagerChooser {
        id: installManagerChooser

        objectName: "installManagerChooser"
        width: 750
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
        width:550
        height: 200
        anchors.centerIn: parent
        anchors.top: parent.top
        visible: false
    }
}
