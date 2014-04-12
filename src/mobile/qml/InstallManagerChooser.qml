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

import QtQuick 2.1

Rectangle {
    id: installManager

    property alias sourceModel: sourceView.model
    property alias categoryModel: categoryView.model
    property alias languageModel: languageView.model
    property alias worksModel: worksView.model
    property alias sourceIndex: sourceView.currentIndex
    property alias categoryIndex: categoryView.currentIndex
    property alias languageIndex: languageView.currentIndex
    property int spacing: 12

    objectName: "installManager"
    color: btStyle.toolbarColor
    border.color: "black"
    border.width: 2

    signal sourceChanged(int index);
    signal categoryChanged(int index);
    signal languageChanged(int index);
    signal workSelected(int index);
    signal cancel();
    signal installRemove();
    signal refreshLists();

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && installManager.visible == true) {
            event.accepted = true;
            installManager.visible = false;
        }
    }

    Grid {
        id:  grid
        columns: 3
        rows: 1
        spacing: installManager.spacing
        width: parent.width - installManager.spacing
        height: installManager.height/3
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: installManager.spacing

        ListTextView {
            id: sourceView
            onItemSelected: {
                sourceChanged(currentIndex)
            }

            title: qsTranslate("InstallManagerChooser","Source")
            width: parent.width/3 - grid.spacing
            height: installManager.height/3
        }

        ListTextView {
            id: categoryView

            title: qsTranslate("InstallManagerChooser","Category")
            width: parent.width/3 - grid.spacing
            height: installManager.height/3
            onItemSelected: {
                categoryChanged(currentIndex)
            }
        }

        ListTextView {
            id: languageView

            title: qsTranslate("InstallManagerChooser","Language")
            width: parent.width/3 - grid.spacing
            height: installManager.height/3
            onItemSelected: {
                languageChanged(currentIndex)
            }
        }
    }

    ListWorksView {
        id: worksView

        title: qsTranslate("InstallManagerChooser","Work")
        width: parent.width - 2 * installManager.spacing
        anchors.top: grid.bottom
        anchors.left: parent.left
        anchors.bottom: installRemoveButton.top
        anchors.margins: installManager.spacing
        onItemSelected: {
            workSelected(index)
        }
    }

    Rectangle {
        id: refreshButton
        width: installManager.width * 3.4 / 10;
        height: btStyle.pixelsPerMillimeterY * 8
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: installRemoveButton.left
        anchors.rightMargin: 10
        border.width: 4
        border.color: "black"

        Text {
            text: qsTranslate("InstallManagerChooser", "Refresh Lists")
            anchors.centerIn: parent
            font.pointSize: btStyle.uiFontPointSize
        }

        MouseArea {
            anchors.fill: parent
            onClicked: installManager.refreshLists()
        }
    }

    Rectangle {
        id: installRemoveButton
        width: installManager.width * 3.8 / 10;
        height: btStyle.pixelsPerMillimeterY * 8
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: cancelButton.left
        anchors.rightMargin: 10
        border.width: 4
        border.color: "black"

        Text {
            text: qsTranslate("InstallManagerChooser", "Install / Remove")
            anchors.centerIn: parent
            font.pointSize: btStyle.uiFontPointSize
        }

        MouseArea {
            anchors.fill: parent
            onClicked: installManager.installRemove()
        }
    }

    Rectangle {
        id: cancelButton
        width: installManager.width * 2.4 / 12;
        height: btStyle.pixelsPerMillimeterY * 8
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: installManager.width * 0.4 / 10;
        border.width: 4
        border.color: "black"

        Text {
            text: qsTranslate("InstallManagerChooser", "Cancel")
            anchors.centerIn: parent
            font.pointSize: btStyle.uiFontPointSize
        }

        MouseArea {
            anchors.fill: parent
            onClicked: installManager.cancel();
        }
    }

}
