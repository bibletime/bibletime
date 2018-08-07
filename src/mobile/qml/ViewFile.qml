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
import QtQuick.Dialogs 1.2
import BibleTime 1.0

Rectangle {
    id: viewFile

    color: btStyle.textBackgroundColor
    anchors.fill: parent

    function open() {
        fileDialog.open();
    }

    BtStyle {
        id: btStyle
    }

    Rectangle {
        id: viewFileTitleBar
        color: btStyle.toolbarColor
        width: parent.width
        height: btStyle.pixelsPerMillimeterY * 7

        Back {
            id: backTool

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            text: "Debug"
            onClicked: {
                viewFile.visible = false;
                debugDialog.visible = true;
            }
        }
    }

    FileInterface {
        id: logFileInterface
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        selectExisting: true
        folder: shortcuts.home
        onAccepted: {
            viewFile.visible = true;
            logFileInterface.source = fileDialog.fileUrl;
            fileViewer.text = logFileInterface.read();
            keyReceiver.forceActiveFocus();
        }
        onRejected: {
            keyReceiver.forceActiveFocus();
            debugDialog.visible = true;
        }
    }

    Flickable {

        anchors.left: parent.left
        anchors.leftMargin: btStyle.pixelsPerMillimeterX
        anchors.top: viewFileTitleBar.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        contentHeight: fileViewer.height

        Text {
            id: fileViewer

            width: parent.width
            wrapMode: Text.Wrap
            font.pointSize: btStyle.uiFontPointSize -1
            color: btStyle.textColor
        }
    }

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && viewFile.visible == true) {
            viewFile.visible = false;
            debugDialog.visible = true;
            event.accepted = true;
        }
    }
}
