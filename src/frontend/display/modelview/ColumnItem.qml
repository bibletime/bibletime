/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

import BibleTime 1.0
import QtQuick 2.2

Item {
    id: columnView

    property alias length: columnText.length
    property color textColor: BtQmlInterface.foregroundColor
    property color textBackgroundColor: BtQmlInterface.backgroundColor
    property color textBackgroundHighlightColor: BtQmlInterface.backgroundHighlightColor
    required property font font

    signal hovered(string link);
    signal setSelection(bool selected, int selectFirstIndex, int selectLastIndex, int posFirst, int posLast)

    function minHeight() {
        var h = columnText.height + (isBlank(displayTitle) ? 0 : columnTitle.height);
        return h;
    }

    function select(posFirst, posLast) {
        columnText.select(posFirst, posLast);
    }

    function deselect() {
        columnText.deselect();
    }

    function selectAll() {
        columnText.selectAll();
    }

    function linkAt(x, y) {
        var textRelativeX = x - columnText.x;
        var textRelativeY = y - columnText.y;
        var url = columnText.linkAt(textRelativeX, textRelativeY);
        return url;
    }

    function positionAt(x, y) {
        var xColumnTextItem = columnText.x;
        var yColumnTextItem = columnText.y;
        return columnText.positionAt(x - xColumnTextItem,y - yColumnTextItem);
    }

    function isBlank(text) {
        var t = text;
        t.trim();
        return t.length === 0;
    }

    Rectangle {
        id: r0

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        color: (listView.backgroundHighlightIndex === index)? textBackgroundHighlightColor: textBackgroundColor
        border.width: (BtQmlInterface.moduleIsWritable(columnView.column))? 1 : 0
        border.color: "gray"
    }

    Text {
        id: columnTitle

        anchors.top: parent.top
        anchors.left: parent.left
        color: columnView.textColor
        font: columnView.font
        text: displayTitle
        textFormat: Text.RichText
        visible: displayTitle != ""
        width: columnView.width
        wrapMode: Text.WordWrap
    }

    TextEdit {
        id: columnText

        anchors.top: columnTitle.visible ? columnTitle.bottom : parent.top
        anchors.left: columnView.left
        color: columnView.textColor
        font: columnView.font
        readOnly: true
        text: {
            var isHtml = displayView.textIsHtml(displayText);
            textFormat = isHtml;
            return displayText;
        }
        visible: listView.columns > 0
        width: columnView.width
        wrapMode: Text.WordWrap
        onLinkHovered: columnView.hovered(link)
    }
}
