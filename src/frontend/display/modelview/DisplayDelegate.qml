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
    id: delegate
    property int spacing: 1.5 * btQmlInterface.pixelsPerMM
    property int textWidth: (listView.width / listView.columns)
    property int vertSpace: 1 * btQmlInterface.pixelsPerMM
    property bool updating: false
    property string selectedText

    function positionAt(x, y, column) {
        var columnViewItem = getColumnItem(column);
        return columnViewItem.positionAt(x - columnViewItem.x, y - columnViewItem.y);
    }

    function linkAt(x, y) {
        var textItem;
        var i;
        for (i = listView.columns; i >= 0; --i) {
            textItem = getColumnItem(i);
            if (x > textItem.x)
                break;
        }
        return textItem.linkAt(textItem.x, textItem.y);
    }

    function hovered(link) {
        if (btQmlInterface.shiftKeyDown())
            return;
        btQmlInterface.setMagReferenceByUrl(link);
        btQmlInterface.activeLink = link;
    }

    function dragStart(index, active) {
        if (active) {
            btQmlInterface.dragHandler(index, btQmlInterface.activeLink);
        }
    }

    function selectSingle(column,posFirst1, posLast1) {
        var item = getColumnItem(column);
        item.select(posFirst1, posLast1);
    }

    function selectFirst(column, posFirst1) {
        var item = getColumnItem(column);
        item.select(posFirst1, item.length);
    }

    function selectLast(column, posLast1) {
        var item = getColumnItem(column);
        item.select(0, posLast1);
    }

    function selectAll() {
        var item = getColumnItem(column);
        item.selectAll();
    }

    function deselect() {
        var item = getColumnItem(column);
        item.deselect();
    }

    function getSelectedText(column) {
        var item = getColumnItem(column);
        return item.selectedText;
    }

    function getDelegateHeight() {
        var h = 30;
        var i;
        for (i = 0; i < listView.columns; ++i) {
            h = Math.max(getColumnItem(i).minHeight(), h);
        }
        return h + vertSpace;
    }

    function getColumnItem(column) {
        if (column === 0)
            return columnView0;
        if (column === 1)
            return columnView1;
        if (column === 2)
            return columnView2;
        return columnView3;
    }

    width: listView.width
    height: getDelegateHeight()

    ColumnItem {
        id: columnView0
        property int column: 0
        property string displayText: text1
        property string displayTitle: title1
        anchors.top: delegate.top
        anchors.bottom: delegate.bottom
        anchors.left: delegate.left
        width: delegate.textWidth
        onHovered: delegate.hovered(link)
    }

    ColumnItem {
        id: columnView1
        property int column: 1
        property string displayText: text2
        property string displayTitle: title2
        anchors.top: delegate.top
        anchors.bottom: delegate.bottom
        anchors.left: columnView0.right
        width: delegate.textWidth
        onHovered: delegate.hovered(link)
    }

    ColumnItem {
        id: columnView2
        property int column: 2
        property string displayText: text3
        property string displayTitle: title3
        anchors.top: delegate.top
        anchors.bottom: delegate.bottom
        anchors.left: columnView1.right
        width: delegate.textWidth
        onHovered: delegate.hovered(link)
    }

    ColumnItem {
        id: columnView3
        property int column: 3
        property string displayText: text4
        property string displayTitle: title4
        anchors.top: delegate.top
        anchors.bottom: delegate.bottom
        anchors.left: columnView2.right
        width: delegate.textWidth
        onHovered: delegate.hovered(link)
    }
}
