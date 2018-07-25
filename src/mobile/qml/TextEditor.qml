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
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import BibleTime 1.0

Rectangle {
    id: textEditor

    color: btStyle.toolbarColor

    function open(text) {
        visible = true;
        textArea.text = text;
    }

    signal editFinished(string newText)

    TextArea {
        id: textArea

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: okButton.top
        anchors.margins: btStyle.pixelsPerMillimeterX * 1
        font.pointSize: btStyle.uiFontPointSize + 2
        textFormat: TextEdit.RichText
        wrapMode: TextEdit.WordWrap
        Keys.forwardTo: [textEditor]
        style: TextAreaStyle {
            textColor: btStyle.textColor
            backgroundColor: btStyle.textBackgroundColor
        }
    }

    Action {
        id: okAction
        text: qsTr("Ok")
        onTriggered: {
            textEditor.visible = false;
            editFinished(textArea.text);
        }
    }

    Button {
        id: okButton

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: btStyle.pixelsPerMillimeterX * 0.5
        anchors.topMargin: btStyle.pixelsPerMillimeterX
        height: {
            var pixel = btStyle.pixelsPerMillimeterY * 5;
            var uiFont = btStyle.uiFontPointSize * 3;
            var mix = pixel * 0.8 + uiFont * 0.35;
            return Math.max(pixel, mix);
        }
        width: parent.width/6
        action: okAction
        style: BtButtonStyle {
        }
    }
}
