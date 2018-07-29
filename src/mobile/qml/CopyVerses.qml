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
    property real cornerRadius: btStyle.pixelsPerMillimeterX * 0.9
    property bool showError: false
    property real rowHeight: {
        var pixel = btStyle.pixelsPerMillimeterY * 5;
        var uiFont = btStyle.uiFontPointSize * 3;
        var mix = pixel * 0.7 + uiFont * 0.3;
        return Math.max(pixel, mix);
    }

    function moduleChoosen() {
        moduleChooser.moduleSelected.disconnect(moduleChoosen);
        copyVerses.moduleName = moduleChooser.selectedModule;
        btWinIfc.moduleName = copyVerses.moduleName
    }

    function chooseRef() {
        chooseReference.finished.disconnect(copyVerses.referenceChoosen);
        chooseReference.finished.connect(copyVerses.referenceChoosen);
        var module = btWinIfc.moduleName;
        var ref = btWinIfc.reference;
        chooseReference.start(module, ref, qsTranslate("Navigation", "Main"));
    }

    function referenceChoosen() {
        if (activeReference == 1)
            copyVerses.reference1 = chooseReference.reference;
        else if (activeReference == 2)
            copyVerses.reference2 = chooseReference.reference;
        copyVerses.showError = btWinIfc.isCopyToLarge(
                    copyVerses.reference1, copyVerses.reference2);
    }

    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.topMargin: 20
    anchors.bottomMargin: btStyle.pixelsPerMillimeterX * 2
    anchors.leftMargin: btStyle.pixelsPerMillimeterX * 2
    anchors.rightMargin: btStyle.pixelsPerMillimeterX * 2
    border.color: btStyle.textColor
    border.width: 6
    color: btStyle.textBackgroundColor
    height: copyVerses.rowHeight * 6;
    width: {
        var width = Math.min(parent.width, parent.height);
        width = width - 5 * anchors.rightMargin
        return width;
    }

    onVisibleChanged: {
        if (visible) {
            copyVerses.showError = false;
            reference1 = theWindow.getReference();
            reference2 = reference1;
            btWinIfc.moduleName = theWindow.getModule();
            btWinIfc.reference = theWindow.getReference();
        }
    }

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && copyVerses.visible == true) {
            copyVerses.visible = false;
             event.accepted = true;
        }
    }

    BtWindowInterface {
        id: btWinIfc
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
        anchors.topMargin: btStyle.pixelsPerMillimeterX * 2.5
        anchors.leftMargin: btStyle.pixelsPerMillimeterX * 2
        anchors.rightMargin: btStyle.pixelsPerMillimeterX * 2
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
                moduleChooser.backText = qsTranslate("Navigation", "Main");
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
                chooseRef();
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
                chooseRef();
            }
        }
    }

    Action {
        id: copyAction

        text: qsTranslate("Copy","Copy")

        onTriggered: {
            var ok = btWinIfc.copy(moduleName, reference1, reference2);
            if (ok)
                copyVerses.visible = false;
        }
    }

    Button {
        id: copyButton

        action: copyAction
        anchors.top: grid.bottom
        anchors.topMargin: btStyle.pixelsPerMillimeterX * 2
        anchors.horizontalCenter: parent.horizontalCenter
        height: copyVerses.rowHeight
        style: BtButtonStyle {
        }
        visible: ! copyVerses.showError
        width: btStyle.pixelsPerMillimeterY * 25
    }

    Text {
        id: message

        anchors.top: grid.bottom
        anchors.topMargin: btStyle.pixelsPerMillimeterX * 3
        anchors.horizontalCenter: parent.horizontalCenter
        color: btStyle.textColor
        font.pointSize: btStyle.uiFontPointSize
        text: qsTranslate("Copy", "Copy size to large.")
        visible: copyVerses.showError
    }

}
