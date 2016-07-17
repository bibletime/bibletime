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

import QtQml.Models 2.2
import QtQuick 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import BibleTime 1.0

Rectangle {
    id: bookmarkManager

    property string module: ""
    property string reference: ""
    property bool allowNewFolders: false
    property int rowHeight: {
        var pixel = btStyle.pixelsPerMillimeterY * 7;
        var uiFont = btStyle.uiFontPointSize * 2.0;
        return Math.max(pixel, uiFont);
    }
    property alias contextMenuModel: bookmarkInterface.contextMenuModel
    property variant index;

    signal newFolder();
    signal bookmarkItemClicked();
    signal openReference(string module, string reference);

    function doContextMenu(action) {
        console.log("Action: ", action, " Index: ", index);
        if (action == "toggleexpand") {
            toggleExpand(index);
            return;
        }

        bookmarkInterface.doContextMenu(action, index);
    }

    function setupContextMenuModel () {
        bookmarkInterface.setupContextMenuModel();
    }

    function addFolder(folderName) {
        bookmarkInterface.addFolder(folderName);
    }

    function addTheReference(reference, moduleName) {
        bookmarkInterface.addBookmark(reference, moduleName);
    }

    function expandAll() {
        var indexes = bookmarkInterface.getBookmarkModelExpandableIndexes();
        for(var i = 0; i <= indexes.length - 1; i++) {
            treeView.expand(indexes[i]);
        }
    }

    function toggleExpand(index) {
        if (treeView.isExpanded(index))
            treeView.collapse(index);
        else
            treeView.expand(index);
    }

    color: btStyle.textBackgroundColor
    anchors.fill: parent
    onVisibleChanged: {
        if (visible)
            treeView.expand(treeView.model.index(0,0));
        newFolderButton.visible = allowNewFolders;
    }

    BtStyle {
        id: btStyle
    }

    BtBookmarkInterface {
        id: bookmarkInterface
    }

    Rectangle {
        id: titleRect

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: titleText.contentHeight * 1.4
        color: btStyle.toolbarColor
        border.color: btStyle.toolbarTextColor
        border.width: 2

        Text {
            id: titleText

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin:btStyle.pixelsPerMillimeterX * 3
            text: qsTranslate("Bookmarks", "Bookmark Manager")
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: btStyle.uiFontPointSize + 4
            color: btStyle.toolbarTextColor
        }
    }

    TreeView {
        id: treeView

        model: bookmarkInterface.bookmarkModel
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: newFolderButton.top
        anchors.bottomMargin: btStyle.pixelsPerMillimeterX * 3
        anchors.top: titleRect.bottom
        alternatingRowColors: false
        backgroundVisible: false
        selectionMode: SelectionMode.SingleSelection

        // See QTBUG-47243
        selection: ItemSelectionModel {
            id: selModel
            model: treeView.model
        }

        onClicked: {
            if (bookmarkInterface.isRoot(index))
                return;

            if (bookmarkInterface.isBookmark(index)) {
                var module = bookmarkInterface.getModule(index);
                var reference = bookmarkInterface.getReference(index);
                openReference(module, reference);
            } else {
                bookmarkManager.toggleExpand(index);
            }
        }

        onPressAndHold: {
            if (bookmarkInterface.isRoot(index))
                return;

            // See QTBUG-47243
            selModel.clearCurrentIndex();
            selModel.setCurrentIndex(index, 0x0002 | 0x0010);
            bookmarkManager.index = index;

            bookmarkInterface.setupContextMenuModel(index);
            if (bookmarkInterface.isBookmark(index)) {
                bookmarkManager.module = bookmarkInterface.getModule(index);
                bookmarkManager.reference = bookmarkInterface.getReference(index);
            }
            bookmarkItemClicked();
        }

        headerDelegate: Rectangle {
            height: 0
            visible: false
        }

        rowDelegate: Rectangle {
            height: bookmarkManager.rowHeight
            property color selectedColor: btStyle.textBackgroundHighlightColor
            color: styleData.selected ? selectedColor : btStyle.textBackgroundColor
        }

        itemDelegate: Item {

            Folder {
                id: icon

                visible: {
                    return ! bookmarkInterface.isBookmark(styleData.index);
                }

                width: parent.height * 0.7
                height: parent.height * 0.7
                anchors.verticalCenter: parent.verticalCenter
                color: btStyle.textColor

            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: icon.right
                color: btStyle.textColor
                elide: styleData.elideMode
                text: {
                    return styleData.value;
                }
                font.pointSize: btStyle.uiFontPointSize
            }
        }

        style: TreeViewStyle {
            indentation: bookmarkManager.rowHeight * 0.5
        }

        TableViewColumn {
            role: "display"
            width: 200
            visible: true
        }
    }

    Button {
        id: newFolderButton

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        action: newFolderAction
        style: BtButtonStyle {
        }
        onVisibleChanged: {
            if (visible)
                height = titleText.height * 1.2
            else
                height = 0;

        }
    }

    Action {
        id: newFolderAction

        text: qsTr("New Folder")
        onTriggered: {
            bookmarkManager.newFolder();
        }
    }
}
