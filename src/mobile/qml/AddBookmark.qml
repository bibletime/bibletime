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
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.2
import BibleTime 1.0

Rectangle {
    id: addBookmark

    property string moduleReference: ""
    property string moduleName: ""
    property string reference: ""
    property string folderName: ""

    signal bookmarkFolders();
    signal addTheBookmark();

    color: btStyle.textBackgroundColor
    anchors.fill: parent

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && addBookmark.visible == true) {
            addBookmark.visible = false;
            event.accepted = true;
        }
    }

    BtStyle {
        id: btStyle
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

        Rectangle {
            id: addBookmarkTitleBar
            color: btStyle.toolbarColor
            width: parent.width
            height: btStyle.pixelsPerMillimeterY * 7

            Back {
                id: backTool

                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                text: qsTranslate("Navigation", "Main")
                onClicked: addBookmark.visible = false;
            }

            Text {
                id: titleText
                color: btStyle.toolbarTextColor
                font.pointSize: btStyle.uiFontPointSize
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.leftMargin: search.spacing
                verticalAlignment: Text.AlignVCenter
                text: qsTranslate("Bookmarks", "Add Bookmark")
            }
        }
    }

    Text {
        id: referenceLabel

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin:btStyle.pixelsPerMillimeterX * 2
        anchors.rightMargin:btStyle.pixelsPerMillimeterX * 2
        anchors.bottom: referenceText.top
        anchors.bottomMargin: btStyle.pixelsPerMillimeterX * 2
        text: qsTranslate("Bookmarks", "Bookmark") + ":"
        elide: Text.ElideMiddle
        font.pointSize: btStyle.uiFontPointSize + 1
        color: btStyle.textColor
    }

    Text {
        id: referenceText

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin:btStyle.pixelsPerMillimeterX * 8
        anchors.rightMargin:btStyle.pixelsPerMillimeterX * 2
        anchors.bottom: folderLabel.top
        anchors.bottomMargin: btStyle.pixelsPerMillimeterX * 10
        text: reference + " (" + moduleName + ")"
        elide: Text.ElideMiddle
        font.pointSize: btStyle.uiFontPointSize + 1
        color: btStyle.textColor
    }

    Text {
        id: folderLabel

        anchors.left: parent.left
        anchors.leftMargin:btStyle.pixelsPerMillimeterX * 2
        anchors.bottom: folderRect.top
        anchors.bottomMargin: btStyle.pixelsPerMillimeterX * 2
        text: qsTranslate("Bookmarks", "Folder") + ":"
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: btStyle.uiFontPointSize + 1
        color: btStyle.textColor

        Action {
            id: chooseAction
            text: qsTr("Choose")
            onTriggered: {
                bookmarkFolders();
            }
        }
    }

    Rectangle {
        id: folderRect

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        height: buttons.height
        anchors.leftMargin:btStyle.pixelsPerMillimeterX * 8
        anchors.rightMargin:btStyle.pixelsPerMillimeterX * 5
        anchors.topMargin: 10
        border.color: btStyle.textColor
        border.width: 2
        color: btStyle.textBackgroundColor

        Text {
            id: folder

            text: addBookmark.folderName
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 10
            font.pointSize: btStyle.uiFontPointSize + 1
            color: btStyle.textColor
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                bookmarkFolders();
            }
        }
    }

    Grid {
        id: buttons

        spacing: btStyle.pixelsPerMillimeterY * 4
        columns: 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: folderRect.bottom
        anchors.topMargin: btStyle.pixelsPerMillimeterX * 12

        Action {
            id: okAction

            text: qsTr("Ok")

            onTriggered: {
                addBookmark.visible = false;
                addBookmark.addTheBookmark();
            }
        }

        Button {
            id: okButton

            height: titleText.height*1.2
            width: addBookmark.width/3.5
            action: okAction
            style: BtButtonStyle {
            }
        }

        Action {
            id: cancelAction

            text: qsTr("Cancel")

            onTriggered: {
                addBookmark.visible = false;
            }
        }

        Button {
            id: cancelButton

            height: titleText.height*1.2
            width: addBookmark.width/3.5
            action: cancelAction
            style: BtButtonStyle {
            }
        }
    }
}
