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

    ListModel {
        id: windowsModel

        ListElement { title: ""; action: "" }
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

    WindowManager {
        id: windowManager

        anchors.top: spacer.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "#646464"

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
        width:650
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
        height: 200
        anchors.centerIn: parent
        anchors.top: parent.top
        visible: false
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
                windowManager.newWindow();
            }
            else if (action == "windows") {
                windowManager.createWindowMenus();
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
            var windowList = windowManager.windows;
            var window = windowList.slice(index, index+1);
            windowList.splice(index,1);
            windowList.unshift(window);
            windowManager.windows = windowList;
            windowManager.layoutWindows();
        }
    }
}
