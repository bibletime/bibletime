import QtQuick 2.0
import QtWebKit 3.0
import BibleTime 1.0

Rectangle {
    id: root

    property int opacitypopup: 0
    property QtObject component: null;
    property Item window: null;
    property var windows: []
    property int tabLayout: 1
    property int autoTile: 2
    property int autoTileHor: 3
    property int autoTileVer: 4
    property int windowLayout: tabLayout

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
        }
    }

    function layout() {
        if (windowLayout == tabLayout) {
            tabbedWindows.z = 1;
            gridWindows.z = -1;
            for (var i=0; i<windows.length; ++i) {
                var window = windows[i];
                window.parent = tabbedWindowsStack;
                window.anchors.fill = tabbedWindowsStack
            }
        }
        else
        {
            tabbedWindows.z = -1;
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
    }


    function selectWindow(n) {
        if (windowLayout == tabLayout) {
            tabbedWindows.current = n;
        }
    }

    function newWindow(type) {
        if (type == "bible")
            component = Qt.createComponent("Window.qml");
        else
            component = Qt.createComponent("BookWindow.qml");

        window = component.createObject(null, {"width": 250, "height": 200});
        window.changeModule();
        if (window == null) {
            // Error Handling
            console.log("Error creating object");
        }
        else {
            windows.push(window)
            layout();
            var curWindow = windows.length -1;
            selectWindow(curWindow);
        }
    }

//    width: 1280
//    height: 800
    width: 980
    height: 500
    color: "black"
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
        color: "black"
    }

    Rectangle {
        id: windowArea

        anchors.top: spacer.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "black"

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

            ListElement { title: QT_TR_NOOP("New Bible Window");        action: "newBibleWindow" }
            ListElement { title: QT_TR_NOOP("New Book Window");         action: "newBookWindow" }
            ListElement { title: QT_TR_NOOP("Window Arrangement");      action: "windowArrangement" }
            ListElement { title: QT_TR_NOOP("Settings");                action: "settings" }
            ListElement { title: QT_TR_NOOP("Gnome Style");             action: "gnomeStyle" }
            ListElement { title: QT_TR_NOOP("Android Style");           action: "androidStyle" }
        }

        Menus {
            id: mainMenus

            Component.onCompleted: menuSelected.connect(mainMenus.doAction)

            function doAction(action) {
                mainMenus.visible = false;
                if (action == "newBibleWindow") {
                    root.newWindow("bible");
                }
                else if (action == "newBookWindow") {
                    root.newWindow("book");
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

            }

            model: mainMenusModel
        }

        ListModel {
            id: windowArrangementModel

            ListElement { title: QT_TR_NOOP("Tabbed");                  action: "tabbed" }
            ListElement { title: QT_TR_NOOP("Auto-tile");               action: "autoTile" }
            ListElement { title: QT_TR_NOOP("Auto-tile horizontally");  action: "autoTileHor" }
            ListElement { title: QT_TR_NOOP("Auto-tile vertically");    action: "autoTileVer" }
        }

        Menus {
            id: windowArrangementMenus

            Component.onCompleted: menuSelected.connect(windowArrangementMenus.doAction)

            function doAction(action) {
                console.log("selected action", action)
                windowArrangementMenus.visible = false;
                if (action == "tabbed") {
                    windowLayout = tabLayout;
                    layout();
                }
                else if (action == "autoTile") {
                    windowLayout = autoTile;
                    layout();
                }
                else if (action == "autoTileHor") {
                    windowLayout = autoTileHor;
                    layout();
                }
                else if (action == "autoTileVer") {
                    windowLayout = autoTileVer;
                    layout();
                }
            }

            model: windowArrangementModel
            anchors.top: parent.bottom
            anchors.right: parent.right
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
}
