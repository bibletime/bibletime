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

Rectangle {
    id: gridChooser

    property int columns: 5
    property int rows: 5
    property int buttonWidth: 100
    property int buttonHeight: 30
    property int space: 10
    property string selected: ""
    property string titleText: ""
    property int maxLength: 0
    property alias model: repeater.model

    signal accepted(string choosenText);
    signal backup();

    color: btStyle.toolbarColor
    height: parent.height
    width: parent.width

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && gridChooser.visible == true) {
            event.accepted = true;
            gridChooser.visible = false;
            backup();
        }
    }

    onVisibleChanged: {
        if ( ! visible)
            return;
        var count = model.length
        if (count < 36)
            count = 36;

        calculateColumns(count);
        buttonWidth = (width - ((columns +1) *space))/columns
        var buttonH = Math.floor((height-(rows*space))/rows) ;
        buttonHeight = Math.max(btStyle.pixelsPerMillimeterY*7.5, buttonH);
        flick.contentY = 0;
    }

    function calculateColumns(count) {
        var aspectRatio = 0.175;
        var columnsF = Math.sqrt(count * width * aspectRatio / height);
        columns = Math.ceil(columnsF);
        rows = Math.ceil((count-0.01)/columns);
    }

    function accept(value) {
        visible = false;
        gridChooser.accepted(value);
    }

    BtStyle {
        id: btStyle
    }

    TitleColorBar {
        id: title

        title: qsTranslate("Choose Reference", titleText)
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Back {
        id: backTool

        anchors.left: parent.left
        anchors.top: title.top
        anchors.bottom: title.bottom
        text: qsTranslate("Navigation", "Main")
        onClicked: {
            gridChooser.visible = false;
            backup();
        }
    }

    Flickable {
        id: flick

        anchors.top: title.bottom
        anchors.left: parent.left
        anchors.leftMargin: space
        width: parent.width
        height: parent.height
        contentHeight: (rows+1)*(buttonHeight+space);
        contentWidth: width
        clip: true

        Grid {
            id: grid

            width: parent.width
            height: parent.contentHeight
            columns: gridChooser.columns
            spacing: gridChooser.space

            Repeater {
                id: repeater

                GridChooserButton {
                    id: button

                    text: modelData
                    textHeight: btStyle.uiFontPointSize
                    buttonWidth: gridChooser.buttonWidth
                    buttonHeight: gridChooser.buttonHeight
                    textColor: btStyle.buttonTextColor
                    buttonColor: (text == gridChooser.selected) ? btStyle.buttonBackground : "white"
                    activeButtonColor: btStyle.buttonTextColor
                    onClicked: gridChooser.accept(text)
                }
            }
        }
    }
}
