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
import QtQuick.Controls 1.2

Rectangle {
    id: installManager

    property alias sourceModel: sourceView.model
    property alias categoryModel: categoryView.model
    property alias languageModel: languageView.model
    property alias worksModel: worksView.model
    property alias sourceIndex: sourceView.currentIndex
    property alias categoryIndex: categoryView.currentIndex
    property alias languageIndex: languageView.currentIndex
    property int spacing: btStyle.pixelsPerMillimeterX

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

        title: qsTranslate("InstallManagerChooser","Document")
        width: parent.width - 2 * installManager.spacing
        anchors.top: grid.bottom
        anchors.left: parent.left
        anchors.bottom: buttonRow.top
        anchors.margins: installManager.spacing
        onItemSelected: {
            workSelected(index)
        }
    }

    Row {
        id: buttonRow

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.bottomMargin: 20
        spacing: (parent.width - refreshButton.width - installRemoveButton.width) / 3
        anchors.leftMargin: (parent.width - refreshButton.width - installRemoveButton.width) / 3

        Action {
            id: refreshAction
            text: qsTranslate("InstallManagerChooser", "Refresh Lists")
            onTriggered: {
                installManager.refreshLists();
            }
        }

        Button {
            id: refreshButton
            height: btStyle.pixelsPerMillimeterY * 7
            width: installManager.width * 4.8 / 10;
            action: refreshAction
            style: BtButtonStyle {
            }
        }

        Action {
            id: installRemoveAction
            text: qsTranslate("InstallManagerChooser", "Install / Remove")
            onTriggered: {
                installManager.installRemove();
            }
        }

        Button {
            id: installRemoveButton
            height: btStyle.pixelsPerMillimeterY * 7
            width: installManager.width * 4.8 / 10;
            action: installRemoveAction
            style: BtButtonStyle {
            }
        }
    }
}
