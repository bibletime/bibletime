/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

import BibleTime 1.0
import QtQuick 2.2

Item {
    id: columnView

    property alias length: columnText.length
    property alias selectedText: columnText.selectedText
    property color textColor: btQmlInterface.foregroundColor
    property color textBackgroundColor: btQmlInterface.backgroundColor

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
        return columnText.linkAt(x, y);
    }

    function positionAt(x, y) {
        var xColumnTextItem = columnText.x;
        var yColumnTextItem = columnText.y;
        return columnText.positionAt(x - xColumnTextItem,y - yColumnTextItem);
    }

    function textIsHtml(dtext) {
        if (dtext.includes("<!DOCTYPE") || dtext.includes("<span"))
            return Text.RichText;
        return Text.PlainText;
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
        color: textBackgroundColor
        border.width: 1
        border.color: "gray"
        visible: {
            return btQmlInterface.moduleIsWritable(columnView.column);
        }
    }

    Text {
        id: columnTitle

        anchors.top: parent.top
        anchors.left: parent.left
        color: columnView.textColor
        font.family: btQmlInterface.fontName0
        font.pointSize: btQmlInterface.fontSize0
        text: displayTitle
        textFormat: Text.RichText
        visible: displayTitle != ""
        width: columnView.width
        wrapMode: Text.WordWrap
    }

    TextEdit {
        id: columnText

        anchors.bottom: parent.bottom
        anchors.left: columnView.left
        color: columnView.textColor
        font.family: btQmlInterface.fontName0
        font.pointSize: btQmlInterface.fontSize0
        readOnly: true
        text: {
            var isHtml = columnView.textIsHtml(displayText);
            textFormat = isHtml;
            return displayText;
        }
        visible: listView.columns > 0
        width: columnView.width
        wrapMode: Text.WordWrap
        onLinkHovered: columnView.hovered(link)
    }
}