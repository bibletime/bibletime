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
import BibleTime 1.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.3

Rectangle {
    id: copyVerses

    property variant theWindow
    property string moduleName: ""
    property string reference1: ""
    property string reference2: ""
    property int activeReference: 0
    property real internalMargins: btStyle.pixelsPerMillimeterX * 0.75
    property real cornerRadius: btStyle.pixelsPerMillimeterX * 0.9;
    property real rowHeight: {
        var pixel = btStyle.pixelsPerMillimeterY * 7;
        var uiFont = btStyle.uiFontPointSize * 3;
        var mix = pixel * 0.7 + uiFont * 0.3;
        return Math.max(pixel, mix);
    }

    function moduleChoosen() {
        moduleChooser.moduleSelected.disconnect(moduleChoosen);
        copyVerses.moduleName = moduleChooser.selectedModule;
        btWinIfc.moduleName = copyVerses.moduleName
    }

    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.topMargin: 20
    anchors.bottomMargin: btStyle.pixelsPerMillimeterX * 2
    anchors.leftMargin: btStyle.pixelsPerMillimeterX * 2
    anchors.rightMargin: btStyle.pixelsPerMillimeterX * 2
    border.color: btStyle.textColor
    border.width: 3
    color: btStyle.textBackgroundColor
    height: {
        if ( message.visible)
            copyVerses.rowHeight * 7.5;
        else
            copyVerses.rowHeight * 6;
    }
    width: {
        var width = Math.min(parent.width, parent.height);
        width = width - 5 * anchors.rightMargin
        return width;
    }

    onVisibleChanged: {
        if (visible) {
            message.visible = false;
            reference1 = theWindow.getReference();
            reference2 = reference1;
            btWinIfc.moduleName = theWindow.getModule();
            btWinIfc.reference = theWindow.getReference();
        }
    }

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && copyVerses.visible == true) {
            copyVerses.visible = false;
        }
    }

    BtWindowInterface {
        id: btWinIfc

        onReferenceChange: {
            if (activeReference == 1)
                copyVerses.reference1 = reference;
            else if (activeReference == 2)
                copyVerses.reference2 = reference;
        }
    }

    Grid {
        id: grid

        property real referenceWidth: {
            var w = grid.width - widestText -spacing;
            return w;
        }
        property real widestText: {
            var w1 = text1.contentWidth;
            var w2 = text2.contentWidth;
            var w3 = text3.contentWidth;
            return Math.max(w1,w2,w3);
        }

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: btStyle.pixelsPerMillimeterX * 4
        anchors.leftMargin: btStyle.pixelsPerMillimeterX * 4
        anchors.rightMargin: btStyle.pixelsPerMillimeterX * 4
        columns: 2
        spacing: btStyle.pixelsPerMillimeterX * 1.5

        Text {
            id: text1

            color: btStyle.textColor
            font.pointSize: btStyle.uiFontPointSize
            height: copyVerses.rowHeight
            text: qsTranslate("copy", "Document") + ":"
            verticalAlignment: Text.AlignVCenter
            width: grid.widestText
        }

        ModuleDisplay {
            id: moduleDisplay

            height: copyVerses.rowHeight
            moduleText: copyVerses.moduleName
            onActivated: {
                moduleChooser.bibleCommentaryOnly = true;
                moduleChooser.moduleSelected.connect(copyVerses.moduleChoosen);
                moduleChooser.visible = true;
            }
            width: grid.referenceWidth
        }

        Text {
            id: text2

            text: qsTranslate("copy", "First") +":"
            font.pointSize: btStyle.uiFontPointSize
            color: btStyle.textColor
            width: grid.widestText
            height: copyVerses.rowHeight
            verticalAlignment: Text.AlignVCenter
        }

        ReferenceDisplay {
            id: referenceDisplay1

            reference: copyVerses.reference1
            width: grid.referenceWidth
            height: copyVerses.rowHeight
            onClicked: {
                copyVerses.activeReference = 1;
            }
        }

        Text {
            id: text3

            text: qsTranslate("copy", "Last") +":"
            font.pointSize: btStyle.uiFontPointSize
            color: btStyle.textColor
            width: grid.widestText
            height: copyVerses.rowHeight
            verticalAlignment: Text.AlignVCenter
        }

        ReferenceDisplay {
            id: referenceDisplay2

            reference: copyVerses.reference2
            width: grid.referenceWidth
            height: copyVerses.rowHeight
            onClicked: {
                copyVerses.activeReference = 2;
            }
        }
    }

    Action {
        id: copyAction

        text: qsTranslate("Copy","Copy")

        onTriggered: {
            var ok = btWinIfc.copy(moduleName, reference1, reference2);
            message.visible = !ok;
            if (ok)
                copyVerses.visible = false;
        }
    }

    Button {
        id: copyButton

        action: copyAction
        anchors.top: grid.bottom
        anchors.topMargin: btStyle.pixelsPerMillimeterX * 3
        anchors.horizontalCenter: parent.horizontalCenter
        height: copyVerses.rowHeight
        style: ButtonStyle {
            label: Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.right: background.right
                color: "black"
                font.pointSize: btStyle.uiFontPointSize
                horizontalAlignment: Text.AlignHCenter
                text: control.text
                verticalAlignment: Text.AlignVCenter
            }
        }
        width: btStyle.pixelsPerMillimeterY * 25
    }

    Text {
        id: message

        anchors.top: copyButton.bottom
        anchors.topMargin: btStyle.pixelsPerMillimeterX * 3
        anchors.horizontalCenter: parent.horizontalCenter
        color: btStyle.textColor
        font.pointSize: btStyle.uiFontPointSize
        text: qsTranslate("Copy", "Copy size to large.")
        visible: false
    }

}
