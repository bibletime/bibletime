import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: root

    property int opacitypopup: 0
    property QtObject component: null;
    property Item window: null;

    ListModel {
        id: windowsModel

        ListElement { title: ""; action: "" }
    }

    function installModules() {
        installManager.openChooser();
    }

    Keys.onReleased: {
        if (event.key == Qt.Key_Back) {
//            console.log("Back button captured - wunderbar !");
            event.accepted = true
            Qt.quit();
        }
    }

    width:  480
    height: 800

    rotation: 0

    MainToolbar {
        id: mainToolbar

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        height: btStyle.pixelsPerMillimeterY * 8
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

    WindowManager {
        id: windowManager

        anchors.top: spacer.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "#646464"

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

    InstallManager {
        id: installManager
    }

    InstallManagerChooser {
        id: installManagerChooser

        objectName: "installManagerChooser"
        width: Math.min(parent.height, parent.width);
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
        height: 300
        anchors.centerIn: parent
        anchors.top: parent.top
        visible: false
    }

    ListModel {
        id: mainMenusModel

        ListElement { title: QT_TR_NOOP("New Window");              action: "newWindow" }
        ListElement { title: QT_TR_NOOP("View Window");             action: "windows" }
        ListElement { title: QT_TR_NOOP("Text Font Size");          action: "textFontSize" }
        ListElement { title: QT_TR_NOOP("User Interface Font Size");action: "uiFontSize" }
        ListElement { title: QT_TR_NOOP("Window Arrangement");      action: "windowArrangement" }

        //ListElement { title: QT_TR_NOOP("Settings");                action: "settings" }
        ListElement { title: QT_TR_NOOP("Bookshelf Manager");       action: "install" }
        //            ListElement { title: QT_TR_NOOP("Gnome Style");             action: "gnomeStyle" }
        //            ListElement { title: QT_TR_NOOP("Android Style");           action: "androidStyle" }
    }

    Menus {
        id: mainMenus

        Component.onCompleted: menuSelected.connect(mainMenus.doAction)

        function doAction(action) {
            mainMenus.visible = false;
            if (action == "newWindow") {
                windowManager.newWindow();
            }
            else if (action == "windows") {
                windowManager.createWindowMenus();
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
            else if (action == "settings") {
                settings.visible = true;
            }
            else if (action == "textFontSize") {
                textFontPointSize.visible = true;
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

    Menus {
        id: windowTitlesMenus

        model: windowsModel
        visible: false
        Component.onCompleted: menuSelected.connect(windowTitlesMenus.doAction)

        function doAction(action) {
            windowTitlesMenus.visible = false;
            var index = Number(action)
            windowManager.setCurrentTabbedWindow(index);
        }
    }

    FontSizeSlider {
        id: uiFontPointSize
        visible: false
        title: "User Interface Font Size"

        onVisibleChanged: {
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

    FontSizeSlider {
        id: textFontPointSize
        visible: false
        title: "Text Font Size"

        onVisibleChanged: {
            if (visible)
            {
                textFontPointSize.current = btStyle.textFontPointSize;
                textFontPointSize.previous = btStyle.textFontPointSize;
            }
        }

        onAccepted: {
            btStyle.textFontPointSize = pointSize;
        }
    }
}
