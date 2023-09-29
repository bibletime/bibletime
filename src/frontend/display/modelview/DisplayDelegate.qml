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
    id: delegate
    property int spacing: 1.5 * BtQmlInterface.pixelsPerMM
    property int textWidth: (listView.width / listView.columns)
    property int vertSpace: 1 * BtQmlInterface.pixelsPerMM
    property bool updating: false
    required property int index
    required property string text0
    required property string text1
    required property string text2
    required property string text3
    required property string text4
    required property string text5
    required property string text6
    required property string text7
    required property string text8
    required property string text9
    required property string title0
    required property string title1
    required property string title2
    required property string title3
    required property string title4
    required property string title5
    required property string title6
    required property string title7
    required property string title8
    required property string title9

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
        return textItem.linkAt(x - textItem.x, y - textItem.y);
    }

    function dragStart(index, active) {
        if (active) {
            BtQmlInterface.dragHandler(index);
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

    function selectAll(column) {
        var item = getColumnItem(column);
        item.selectAll();
    }

    function deselect(column) {
        var item = getColumnItem(column);
        item.deselect();
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
        return [columnView0, columnView1, columnView2, columnView3,
                columnView4, columnView5, columnView6, columnView7,
                columnView8, columnView9][column];
    }

    width: listView.width
    height: getDelegateHeight()

    ColumnItem {
        id: columnView0
        property int column: 0
        property string displayText: text0
        property string displayTitle: title0
        anchors.top: delegate.top
        anchors.bottom: delegate.bottom
        anchors.left: delegate.left
        font: BtQmlInterface.font0
        width: delegate.textWidth
        onHovered: BtQmlInterface.setHoveredLink(link)
    }

    ColumnItem {
        id: columnView1
        property int column: 1
        property string displayText: text1
        property string displayTitle: title1
        anchors.top: delegate.top
        anchors.bottom: delegate.bottom
        anchors.left: columnView0.right
        font: BtQmlInterface.font1
        width: delegate.textWidth
        onHovered: BtQmlInterface.setHoveredLink(link)
    }

    ColumnItem {
        id: columnView2
        property int column: 2
        property string displayText: text2
        property string displayTitle: title2
        anchors.top: delegate.top
        anchors.bottom: delegate.bottom
        anchors.left: columnView1.right
        font: BtQmlInterface.font2
        width: delegate.textWidth
        onHovered: BtQmlInterface.setHoveredLink(link)
    }

    ColumnItem {
        id: columnView3
        property int column: 3
        property string displayText: text3
        property string displayTitle: title3
        anchors.top: delegate.top
        anchors.bottom: delegate.bottom
        anchors.left: columnView2.right
        font: BtQmlInterface.font3
        width: delegate.textWidth
        onHovered: BtQmlInterface.setHoveredLink(link)
    }

    ColumnItem {
        id: columnView4
        property int column: 4
        property string displayText: text4
        property string displayTitle: title4
        anchors.top: delegate.top
        anchors.bottom: delegate.bottom
        anchors.left: columnView3.right
        font: BtQmlInterface.font4
        width: delegate.textWidth
        onHovered: BtQmlInterface.setHoveredLink(link)
    }

    ColumnItem {
        id: columnView5
        property int column: 5
        property string displayText: text5
        property string displayTitle: title5
        anchors.top: delegate.top
        anchors.bottom: delegate.bottom
        anchors.left: columnView4.right
        font: BtQmlInterface.font5
        width: delegate.textWidth
        onHovered: BtQmlInterface.setHoveredLink(link)
    }

    ColumnItem {
        id: columnView6
        property int column: 6
        property string displayText: text6
        property string displayTitle: title6
        anchors.top: delegate.top
        anchors.bottom: delegate.bottom
        anchors.left: columnView5.right
        font: BtQmlInterface.font6
        width: delegate.textWidth
        onHovered: BtQmlInterface.setHoveredLink(link)
    }

    ColumnItem {
        id: columnView7
        property int column: 7
        property string displayText: text7
        property string displayTitle: title7
        anchors.top: delegate.top
        anchors.bottom: delegate.bottom
        anchors.left: columnView6.right
        font: BtQmlInterface.font7
        width: delegate.textWidth
        onHovered: BtQmlInterface.setHoveredLink(link)
    }

    ColumnItem {
        id: columnView8
        property int column: 8
        property string displayText: text8
        property string displayTitle: title8
        anchors.top: delegate.top
        anchors.bottom: delegate.bottom
        anchors.left: columnView7.right
        font: BtQmlInterface.font8
        width: delegate.textWidth
        onHovered: BtQmlInterface.setHoveredLink(link)
    }

    ColumnItem {
        id: columnView9
        property int column: 9
        property string displayText: text9
        property string displayTitle: title9
        anchors.top: delegate.top
        anchors.bottom: delegate.bottom
        anchors.left: columnView8.right
        font: BtQmlInterface.font9
        width: delegate.textWidth
        onHovered: BtQmlInterface.setHoveredLink(link)
    }
}
