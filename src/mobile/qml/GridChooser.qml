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

import QtQuick 2.1
import BibleTime 1.0

Rectangle {
    id: gridChooser

    property int columns: 5
    property int rows: 5
    property int buttonWidth: 100
    property int buttonHeight: 30
    property int topMargin: 6
    property int leftMargin: 4
    property int titleHeight: 4
    property int space: 8
    property string selected: ""
    property string titleText: ""
    property int maxLength: 0

    signal accepted(string choosenText);
    signal backup();

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && gridChooser.visible == true) {
            event.accepted = true;
            gridChooser.visible = false;
            backup();
        }
    }

    onVisibleChanged: {

        var count = gridChooserModel.length
        if (count < 36)
            count = 36;

        calculateColumns(count);

        buttonWidth = (width/columns) - space;
        buttonHeight = Math.floor((height-(rows*space))/rows) ;
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

    Text {
        id: title

        text: titleText
        font.pointSize: btStyle.uiFontPointSize
        height: titleHeight
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    BtStyle {
        id: btStyle
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        enabled: gridChooser.opacity
    }

    Rectangle {
        anchors.fill: parent
        color: btStyle.buttonBackground
    }

    Rectangle {
        id: topSpace

        width: leftMargin
        height: topMargin
        color: btStyle.buttonBackground
    }

    Grid {
        id: grid

        anchors.top: topSpace.bottom
        anchors.bottom: bottom.top
        anchors.left: topSpace.right
        anchors.right: parent.right
        columns: gridChooser.columns
        spacing: gridChooser.space

        Repeater {
            id: repeater

            model: gridChooserModel

            GridChooserButton {
                id: buttonX

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
