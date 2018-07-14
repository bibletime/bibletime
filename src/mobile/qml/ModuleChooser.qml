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
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import BibleTime 1.0

Rectangle {
    id: moduleChooser

    property alias categoryModel: categoryView.model
    property alias languageModel: languageView.model
    property alias worksModel: worksView.model
    property alias categoryIndex: categoryView.currentIndex
    property alias languageIndex: languageView.currentIndex
    property alias moduleIndex: worksView.currentIndex
    property int lastCategoryIndex: 0
    property int lastLanguageIndex: 0
    property int spacing: 4
    property string selectedModule: ""
    property string selectedCategory: ""
    property string backText: ""
    property bool bibleCommentaryOnly: false

    objectName: "moduleChooser"

    signal canceled

    function requestModuleUnlockKey() {
        unlockDlg.visible = true;
    }

    color: "lightgray"
    border.color: "black"
    border.width: 2

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && moduleChooser.visible == true) {
            event.accepted = true;
            moduleChooser.visible = false;
            unlockDlg.visible = false;
        }
    }

    onVisibleChanged: {
        if (visible == true) {
            moduleInterface.setBibleCommentaryOnly(bibleCommentaryOnly);
            moduleInterface.updateCategoryAndLanguageModels();
            categoryIndex = lastCategoryIndex;
            languageIndex = lastLanguageIndex;
        }
    }

    onCategoryIndexChanged: {
        if (visible == true) {
            moduleInterface.updateWorksModel();
        }
    }

    onLanguageIndexChanged: {
        if (visible == true) {
            moduleInterface.updateWorksModel();
        }
    }

    signal categoryChanged(int index);
    signal languageChanged(int index);
    signal moduleSelected();

    Rectangle {
        id: unlockDlg

        z: 100
        visible: false
        color: btStyle.textBackgroundColor
        anchors.fill: parent

        signal finished(string unlockKey);

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        Text {
            id: message
            text: "This document is locked.\nYou must enter the unlock key."
            horizontalAlignment: Text.AlignHCenter
            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: btStyle.uiFontPointSize
            width: parent.width
            height: contentHeight * 1.1
            color: btStyle.textColor
        }

        TextField {
            id: input

            anchors.top: message.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width/2
            font.pointSize: btStyle.uiFontPointSize
            textColor: btStyle.textColor
            style: TextFieldStyle {
                textColor: btStyle.textColor
                background: Rectangle {
                    radius: 6
                    anchors.fill: parent
                    border.color: btStyle.textColor
                    border.width: 1
                    color: btStyle.textBackgroundColor
                }
            }
        }

        Action {
            id: unlockAction
            text: QT_TR_NOOP("Unlock")
            onTriggered: {
                unlockDlg.visible = false;
                moduleInterface.unlock(selectedModule, input.text);
                if (moduleInterface.isLocked(selectedModule)) {
                    console.log("module did not unlock");
                    return;
                }
                bibleCommentaryOnly = false;
                moduleSelected();
                moduleChooser.visible = false;
            }
        }

        Button {
            id: unlockButton
            anchors.top: input.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: btStyle.pixelsPerMillimeterY * 3
            height: btStyle.pixelsPerMillimeterY * 7
            width: btStyle.pixelsPerMillimeterY * 25
            action: unlockAction
            style: BtButtonStyle {
            }
        }
    }


    ModuleInterface {
        id: moduleInterface

        onRequestCurrentCategory: {
            moduleInterface.setCurrentCategory(categoryView.currentIndex)
        }
        onRequestCurrentLanguage: {
            moduleInterface.setCurrentLanguage(languageView.currentIndex)
        }
    }

    BtStyle {
        id: beStyle
    }

    Rectangle {
        id: newWindowTitleBar
        color: btStyle.toolbarColor
        width: parent.width
        height: btStyle.pixelsPerMillimeterY * 7

        Back {
            id: backTool

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            text: moduleChooser.backText
            onClicked: {
                moduleChooser.canceled();
            }
        }

        Text {
            id: title
            color: btStyle.toolbarTextColor
            font.pointSize: btStyle.uiFontPointSize
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.leftMargin: search.spacing
            verticalAlignment: Text.AlignVCenter
            text: qsTranslate("Search", "Choose Work")
        }
    }

    Grid {
        id:  grid
        columns: 2
        rows: 1
        spacing: parent.spacing
        width: parent.width - moduleChooser.spacing
        height: parent.height/2.5
        anchors.left: parent.left
        anchors.top: newWindowTitleBar.bottom
        anchors.margins: parent.spacing

        ListTextView {
            id: categoryView

            title: qsTranslate("ModuleChooser", "Category")
            width: grid.width/2 - grid.spacing
            height: grid.height
            model: moduleInterface.categoryModel
            onItemSelected: {
                categoryChanged(currentIndex)
            }
        }

        ListTextView {
            id: languageView

            title: qsTranslate("ModuleChooser", "Language")
            width: grid.width/2 - grid.spacing
            height: grid.height
            model: moduleInterface.languageModel
            onItemSelected: {
                languageChanged(currentIndex);
            }
        }
    }

    ListTextView {
        id: worksView

        title: qsTranslate("ModuleChooser", "Work")
        width: parent.width - 2 * parent.spacing
        anchors.top: grid.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: moduleChooser.spacing
        highlight: false
        model: moduleInterface.worksModel
        onItemSelected: {
            selectedModule = moduleInterface.module(index);
            selectedCategory = moduleInterface.englishCategory(index);
            if (moduleInterface.isLocked(selectedModule)) {
                requestModuleUnlockKey();
                return;
            }
            bibleCommentaryOnly = false;
            moduleSelected();
            moduleChooser.visible = false;
        }
    }
}

