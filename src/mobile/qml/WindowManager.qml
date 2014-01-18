import QtQuick 2.1
import BibleTime 1.0


Rectangle {
    id: windowManager

    objectName: "WindowManager"

    property var windows: []

    property int autoTileVer: 1
    property int autoTileHor: 2
    property int single: 3  // Tile in Desktop
    // 4 is Manual in Desktop
    property int autoTile: 5
    property int tabLayout: 6

    property int windowArrangement: single

    function setCurrentTabbedWindow(index) {
        tabbedWindows.current = index;
    }

    function setWindowArrangement(arrangement) {
        if (arrangement < single || arrangement > autoTileVer)
            return;
        windowArrangement = arrangement;
        layoutWindows();
    }

    function createWindowMenus() {
        windowsModel.clear();
        for (var i=0; i<windows.length; ++i) {
            var window = windows[i];
            windowsModel.append (
                        { title: window.title, action: i.toString() }
                        )
        }
        windowTitlesMenus.model = windowsModel
        windowTitlesMenus.visible = true;
    }


    function newWindow() {
        moduleChooser.moduleSelected.connect(openWindowSlot);
        moduleChooser.visible = true;
    }

    function openWindowSlot() {
        moduleChooser.moduleSelected.disconnect(openWindowSlot);
        openWindow(moduleChooser.selectedCategory, moduleChooser.selectedModule, "")
    }

    function openWindow(category, module, key) {
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
        if (key !== "")
            window.setKey(key);
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
        }
    }

    function arrangeSingleWindow() {
        tabbedWindows.z = 1;
        tabbedWindows.tabVisible = false;
        for (var i=0; i<windows.length; ++i) {
            var window = windows[i];
            window.parent = tabbedWindowsStack;
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
    }

    function selectWindow(n) {
        if (windowArrangement == tabLayout || windowArrangement == single) {
            tabbedWindows.current = n;
        }
    }

//    anchors.top: spacer.bottom
//    anchors.left: parent.left
//    anchors.right: parent.right
//    anchors.bottom: parent.bottom
//    color: "#646464"

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
                    width: {
                        calculateTabWidth()
                    }
                    height: btStyle.pixelsPerMillimeterY * 6

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
                        border.width: 3

                        Text {
                            id: tabText

                            horizontalAlignment: Qt.AlignHCenter;
                            verticalAlignment: Qt.AlignVCenter
                            anchors.fill: parent
                            anchors.topMargin: 6
                            font.pointSize: btStyle.uiFontPointSize -3
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
