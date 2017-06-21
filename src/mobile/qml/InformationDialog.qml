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

import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

Rectangle {
    id: informationDialog

    property string text: ""
    border.color: btStyle.textColor
    border.width: 3
    color: btStyle.textBackgroundColor

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape)  && informationDialog.visible == true) {
            event.accepted = true;
            informationDialog.visible = false;
        }
    }
    TextArea {
        id: textArea

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right:parent.right
        anchors.bottom: closeButton.top
        anchors.margins: btStyle.pixelsPerMillimeterX * 2
        font.pointSize: btStyle.uiFontPointSize + 1
        readOnly: true
        textMargin: btStyle.pixelsPerMillimeterX
        text: informationDialog.text
        textFormat: TextEdit.RichText
        style: TextAreaStyle {
            textColor: btStyle.textColor
            backgroundColor: btStyle.textBackgroundColor
        }
    }

    Action {
        id: closeAction
        text: qsTr("Close")
        onTriggered: {
            informationDialog.visible = false;
        }
    }

    Button {
        id: closeButton

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: btStyle.pixelsPerMillimeterX * 2
        anchors.topMargin: btStyle.pixelsPerMillimeterX * 2
        height: {
            var pixel = btStyle.pixelsPerMillimeterY * 5;
            var uiFont = btStyle.uiFontPointSize * 3;
            var mix = pixel * 0.8 + uiFont * 0.35;
            return Math.max(pixel, mix);
        }
        width: parent.width/3
        action: closeAction
        style: BtButtonStyle {
        }
    }
}
