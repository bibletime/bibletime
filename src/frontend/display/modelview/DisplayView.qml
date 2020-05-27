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

Rectangle {
    id: display

    property int column: 0
    property int contextMenuIndex: btQmlInterface.contextMenuIndex
    property int dragDistance: 8
    property int indexFirst: -1
    property int indexLast: -1
    property int mouseMovedX: 0
    property int mouseMovedY: 0
    property int mousePressedX: 0
    property int mousePressedY: 0
    property point mouseLR
    property point mouseUL
    property int textPosFirst: -1
    property int textPosLast: -1

    function saveContextMenuIndex(x, y) {
        contextMenuIndex = listView.indexAt(x,y+listView.contentY);
    }

    function leftMouseMove(x, y) {
        if (mousePressedX < 0)
            return;
        if ((Math.abs(mousePressedX - x) < dragDistance) && (Math.abs(mousePressedY - y) < dragDistance)) {
            return;
        }
        mouseMovedX = x;
        mouseMovedY = y;
        column = Math.floor(mousePressedX / (listView.width / listView.columns));
        sortPoints();
        selectItems();
        btQmlInterface.selectByIndex(indexFirst, indexLast, column, textPosFirst, textPosLast);
    }

    function leftMousePress(x, y) {
        var item1 = listView.itemAt( x,y + listView.contentY);

        // Transform to item relative point
        var xItem = x - item1.x + listView.contentX;
        var yItem = y - item1.y + listView.contentY;

        // If not a cross reference link, save cursor position for later use
        var  url = item1.linkAt(xItem, yItem);
        if (url === "" || url.includes("lemmamorph") || url.includes("footnote")) {
            // Start selection of items
            mousePressedX = x;
            mousePressedY = y;
            btQmlInterface.deSelect();
            return;
        }

        // Start drag operation for cross reference link
        var index = listView.indexAt( x,y + listView.contentY);
        btQmlInterface.dragHandler(index, url);
        return;
    }

    function leftMouseRelease(x, y) {
        if (mousePressedX < 0)
            return;
        if ((Math.abs(mousePressedX - x) > dragDistance) && (Math.abs(mousePressedY - y) > dragDistance)) {
            return;
        }
        openPersonalCommentary(mousePressedX, mousePressedY);
    }

    function sortPoints() {
        if (mousePressedX < mouseMovedX) {
            if (mousePressedY < mouseMovedY) {
                mouseUL = Qt.point(mousePressedX, mousePressedY);
                mouseLR = Qt.point(mouseMovedX, mouseMovedY);
            } else {
                mouseUL = Qt.point(mousePressedX, mouseMovedY);
                mouseLR = Qt.point(mouseMovedX, mousePressedY);
            }
        } else {
            if (mousePressedY < mouseMovedY) {
                mouseUL = Qt.point(mouseMovedX, mousePressedY);
                mouseLR = Qt.point(mousePressedX, mouseMovedY);
            } else {
                mouseUL = Qt.point(mouseMovedX, mouseMovedY);
                mouseLR = Qt.point(mousePressedX, mousePressedY);
            }
        }
    }

    function selectItems() {
        var itemFirst = listView.itemAt(mouseUL.x, mouseUL.y + listView.contentY);
        var itemLast = listView.itemAt(mouseLR.x, mouseLR.y + listView.contentY);
        if (itemFirst === null || itemLast === null)
            return;

        indexFirst = listView.indexAt(mouseUL.x, mouseUL.y + listView.contentY);
        indexLast = listView.indexAt(mouseLR.x, mouseLR.y + listView.contentY);

        var textXFirst = listView.contentX + mouseUL.x - itemFirst.x;
        var textYFirst = listView.contentY + mouseUL.y - itemFirst.y;
        textPosFirst = itemFirst.positionAt(textXFirst, textYFirst, column);

        var textXLast = listView.contentX + mouseLR.x - itemLast.x;
        var textYLast = listView.contentY + mouseLR.y - itemLast.y;
        textPosLast = itemLast.positionAt(textXLast, textYLast, column);
    }

    function openPersonalCommentary(x, y) {
        var index = listView.indexAt( x,y + listView.contentY);
        var column = Math.floor(x / (listView.width / listView.columns));
        listView.startEdit(index, column);
    }

    function updateReferenceText() {
        listView.updateReferenceText();
    }

    function scroll(value) {
        listView.scroll(value);
    }

    width: 10
    height: 10
    color: btQmlInterface.backgroundColor

    BtQmlInterface {
        id: btQmlInterface

        onPageDownChanged: {
            listView.scroll(listView.height * 0.8);
            updateReferenceText();
        }
        onPageUpChanged: {
            listView.scroll(listView.height * -0.8);
            updateReferenceText();
        }
        onPositionItemOnScreen: {
            listView.positionViewAtIndex(index, ListView.Contain);
            updateReferenceText();
        }
    }


    ListView {
        id: listView

        property color textColor: btQmlInterface.foregroundColor
        property color textBackgroundColor: btQmlInterface.backgroundColor
        property int columns: btQmlInterface.numModules
        property int savedRow: 0
        property int savedColumn: 0

        function scroll(value) {
            var y = contentY;
            contentY = y + value;
        }

        function startEdit(row, column) {
            if (!btQmlInterface.moduleIsWritable(column))
                return
            savedRow = row;
            savedColumn = column;
            btQmlInterface.openEditor(row, column);
        }

        function finishEdit(newText) {
            btQmlInterface.setRawText(savedRow, savedColumn, newText)
        }

        function updateReferenceText() {
            var index = indexAt(contentX,contentY+30);
            btQmlInterface.changeReference(index);
        }

        clip: true
        anchors.fill: parent
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.topMargin: 0
        anchors.bottomMargin: 0
        focus: true
        maximumFlickVelocity: 900
        model: btQmlInterface.textModel
        spacing: 2
        highlightFollowsCurrentItem: true
        currentIndex: btQmlInterface.currentModelIndex
        onCurrentIndexChanged: {
            positionViewAtIndex(currentIndex,ListView.Beginning)
        }
        onMovementEnded: {
            updateReferenceText();
        }

        delegate: Component {
            Rectangle {
                id: delegate
                property int spacing: 1.5 * btQmlInterface.pixelsPerMM
                property int textWidth: (listView.width / listView.columns) - (spacing *  ((listView.columns+1)/listView.columns)  )
                property int vertSpace: 2 * btQmlInterface.pixelsPerMM
                property bool updating: false
                property string selectedText

                function positionAt(x, y, column) {
                    var textItem
                    if (column === 0)
                        textItem = column0Text;
                    else if (column === 1)
                        textItem = column1Text;
                    else if (column === 2)
                        textItem = column2Text;
                    else
                        textItem = column3Text;

                    var xItem = textItem.x;
                    var position = textItem.positionAt(x - xItem, y);
                    return position;
                }

                function linkAt(x, y) {
                    var textItem;
                    if (column3Text.visible && x >= column3Text.x)
                        textItem = column3Text;
                    else if (column2Text.visible && x >= column2Text.x)
                        textItem = column2Text;
                    else if (column1Text.visible && x >= column1Text.x)
                        textItem = column1Text;
                    else
                        textItem = column0Text;

                    var xTextItem = x - textItem.x;
                    var yTextItem = y - textItem.y;
                    return textItem.linkAt(xTextItem, yTextItem);
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

                function getColumnItem(column) {
                    if (column === 0)
                        return column0Text;
                    if (column === 1)
                        return column1Text;
                    if (column === 2)
                        return column2Text;
                    return column3Text;
                }

                function selectSingle(item,posFirst, posLast, columnSelected) {
                    item.select(posFirst, posLast);
                }

                function selectFirst(item, posFirst, columnSelected) {
                    item.select(posFirst, item.length);
                }                    color: listView.textBackgroundColor


                function selectLast(item, posLast, columnSelected) {
                    item.select(0, posLast);
                }

                function selectAll(item, columnSelected) {
                    item.selectAll();
                }

                function setSelection(selected, selectFirstIndex, selectLastIndex, posFirst, posLast) {
                    if (updating)
                        return;
                    updating = true;

                    if (selected) {
                        var item = getColumnItem(columnSelected);
                        if (selectFirstIndex && selectLastIndex)
                            selectSingle(item, posFirst, posLast, columnSelected);
                        else if (selectFirstIndex)
                            selectFirst(item, posFirst, columnSelected)
                        else if (selectLastIndex)
                            selectLast(item, posLast, columnSelected)
                        else
                            selectAll(item, columnSelected);
                        btQmlInterface.saveSelectedText(index, item.selectedText)
                    }
                    updating = false;
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

                width: listView.width
                height: {
                    var h = 30;
                    if (listView.columns == 1)
                        h = Math.max(column0Text.height,20) + vertSpace;
                    if (listView.columns == 2)
                        h = Math.max(column0Text.height, column1Text.height) + vertSpace;
                    if (listView.columns == 3)
                        h = Math.max(column0Text.height, column1Text.height, column2Text.height) + vertSpace;
                    if (listView.columns == 4)
                        h = Math.max(column0Text.height, column1Text.height,
                                        column2Text.height, column3Text.height) + vertSpace;
                    if (! isBlank(title1))
                        h = h + column0Title.height;
                    return h;
                }

                Item {
                    id: space1
                    height: 10
                    width: parent.spacing
                    anchors.left: parent.left
                    anchors.top: parent.top
                }

                Rectangle {
                    id: r0

                    anchors.top: delegate.top
                    anchors.bottom: delegate.bottom
                    anchors.left: column0Text.left
                    anchors.right: column0Text.right
                    color: listView.textBackgroundColor
                    border.width: 1
                    border.color: "lavender"
                    visible: {
                        return btQmlInterface.moduleIsWritable(0);
                    }
                }

                Text {
                    id: column0Title

                    anchors.top: parent.top
                    anchors.left: space1.right
                    width: parent.textWidth
                    text: title1
                    textFormat: Text.RichText
                    visible: ! isBlank(title1)
                }

                TextEdit {
                    id: column0Text

                    anchors.top: isBlank(title1) ? parent.top : column0Title.bottom
                    anchors.left: space1.right
                    width: parent.textWidth
                    text: {
                        var isHtml = delegate.textIsHtml(text1);
                        textFormat = isHtml;
                        return text1;
                    }
                    readOnly: true
                    color: listView.textColor
                    font.family: btQmlInterface.fontName0
                    font.pointSize: btQmlInterface.fontSize0
                    wrapMode: Text.WordWrap
                    visible: listView.columns > 0
                    z:1
                    onLinkHovered: delegate.hovered(link)
                    onTextChanged: {
                        delegate.setSelection(selected, selectFirstIndex, selectLastIndex, posFirst, posLast)
                    }
                }

                Item {
                    id: space2
                    height: 10
                    width: parent.spacing
                    anchors.left: column0Text.right
                    anchors.top: parent.top
                }

                Rectangle {
                    id: r1

                    anchors.top: delegate.top
                    anchors.bottom: delegate.bottom
                    anchors.left: column1Text.left
                    anchors.right: column1Text.right
                    color: listView.textBackgroundColor
                    border.width: 1
                    border.color: "lavender"
                    visible: {
                        return btQmlInterface.moduleIsWritable(1);
                    }
                }

                Text {
                    id: column1Title

                    anchors.top: parent.top
                    anchors.left: space2.right
                    width: parent.textWidth
                    text: title2
                    textFormat: Text.RichText
                    visible: ! isBlank(title2)
                }

                TextEdit {
                    id: column1Text

                    anchors.top: isBlank(title2) ? parent.top : column1Title.bottom
                    anchors.left: space2.right
                    anchors.leftMargin: 0
                    width: parent.textWidth
                    text: {
                        var isHtml = delegate.textIsHtml(text2);
                        textFormat = isHtml;
                        return text2;
                    }
                    readOnly: true
                    color: listView.textColor
                    font.family: btQmlInterface.fontName1
                    font.pointSize: btQmlInterface.fontSize1
                    wrapMode: Text.WordWrap
                    visible: listView.columns > 1
                    onLinkHovered: delegate.hovered(link)
                    onTextChanged: {
                        delegate.setSelection(selected, selectFirstIndex, selectLastIndex, posFirst, posLast)
                    }
                }

                Item {
                    id: space3
                    height: 10
                    width: parent.spacing
                    anchors.left: column1Text.right
                    anchors.top: parent.top
                }

                Rectangle {
                    id: r2
                    anchors.top: delegate.top
                    anchors.bottom: delegate.bottom
                    anchors.left: column2Text.left
                    anchors.right: column2Text.right
                    color: listView.textBackgroundColor
                    border.width: 1
                    border.color: "lavender"
                    visible: {
                        return btQmlInterface.moduleIsWritable(2);
                    }
                }

                Text {
                    id: column2Title

                    anchors.top: parent.top
                    anchors.left: space3.right
                    width: parent.textWidth
                    text: title3
                    textFormat: Text.RichText
                    visible: ! isBlank(title3)
                }

                TextEdit {
                    id: column2Text

                    anchors.top: isBlank(title3) ? parent.top : column2Title.bottom
                    anchors.left: space3.right
                    anchors.leftMargin: 0
                    width: parent.textWidth
                    text: {
                        var isHtml = delegate.textIsHtml(text3);
                        textFormat = isHtml;
                        return text3;
                    }
                    readOnly: true
                    color: listView.textColor
                    font.family: btQmlInterface.fontName2
                    font.pointSize: btQmlInterface.fontSize2
                    wrapMode: Text.WordWrap
                    visible: listView.columns > 2
                    z: 1
                    onLinkHovered: delegate.hovered(link)
                    onTextChanged: {
                        delegate.setSelection(selected, selectFirstIndex, selectLastIndex, posFirst, posLast)
                    }
                }

                Item {
                    id: space4
                    height: 10
                    width: parent.spacing
                    anchors.left: column2Text.right
                    anchors.top: parent.top
                }

                Rectangle {
                    id: r3
                    anchors.top: delegate.top
                    anchors.bottom: delegate.bottom
                    anchors.left: column3Text.left
                    anchors.right: column3Text.right
                    color: listView.textBackgroundColor
                    border.width: 1
                    border.color: "lavender"
                    visible: {
                        return btQmlInterface.moduleIsWritable(3);
                    }
                }

                Text {
                    id: column3Title

                    anchors.top: parent.top
                    anchors.left: space4.right
                    width: parent.textWidth
                    text: title4
                    textFormat: Text.RichText
                    visible: ! isBlank(title4)
                }

                TextEdit {
                    id: column3Text

                    anchors.top: isBlank(title4) ? parent.top : column3Title.bottom
                    anchors.left: space4.right
                    anchors.leftMargin: 0
                    width: parent.textWidth
                    text: {
                        var isHtml = delegate.textIsHtml(text4);
                        textFormat = isHtml;
                        return text4;
                    }
                    readOnly: true
                    color: listView.textColor
                    font.family: btQmlInterface.fontName3
                    font.pointSize: btQmlInterface.fontSize3
                    wrapMode: Text.WordWrap
                    visible: listView.columns > 3
                    z: 1
                    onLinkHovered: delegate.hovered(link)
                    onTextChanged: {
                        delegate.setSelection(selected, selectFirstIndex, selectLastIndex, posFirst, posLast)
                    }
                }
            }
        }
    }
}


