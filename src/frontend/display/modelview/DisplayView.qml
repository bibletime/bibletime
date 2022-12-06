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
import QtQuick 2.10

Rectangle {
    id: display

    property alias contextMenuIndex: btQmlInterface.contextMenuIndex
    property alias contextMenuColumn: btQmlInterface.contextMenuColumn

    // Mouse movement properties
    property bool mousePressedAndMoving: false
    property int mousePressedX: 0
    property int mousePressedY: 0
    property bool draggingInProgress: false
    property bool selectionInProgress: false
    property string pressedLink: ""

    // Text selection properties
    property int selectedTextColumn: 0
    property int indexFirst: -1     // Index of first delegate item with selected text
    property int indexLast: -1      // Index of last delegate item with selected text
    property Item itemFirst: null   // First delegate item with selected text
    property Item itemLast: null    // Last delegate item with selected text
    property int textPosFirst: -1   // Position of first selected character
    property int textPosLast: -1    // Position of last selected character

    function saveContextMenuIndex(x, y) {
        contextMenuColumn = Math.floor(x / (listView.width / listView.columns));
        contextMenuIndex = listView.indexAt(x,y+listView.contentY);
    }

    function leftMousePress(x, y) {
        // Save cursor position for later use
        mousePressedX = x;
        mousePressedY = y;

        if (btQmlInterface.hasSelectedText()) {
            deselectByItem();
            return;
        }

        var delegateItem = listView.itemAt( x, y + listView.contentY);
        if (delegateItem === null)
            return;
        // Transform to delegate relative point
        var delegateX = x - delegateItem.x + listView.contentX;
        var delegateY = y - delegateItem.y + listView.contentY;
        pressedLink = delegateItem.linkAt(delegateX, delegateY);
    }

    function leftMouseMove(x, y) {
        draggingInProgress = false;
        selectionInProgress = false;
        // Do nothing unless mouse moved some distance from pressed location
        if (!mousePressedAndMoving) {
            const dragDistance = 8;
            if ((Math.abs(mousePressedX - x) < dragDistance)
                && (Math.abs(mousePressedY - y) < dragDistance))
                return;
            mousePressedAndMoving = true;
        }
        if (isBibleReference(pressedLink)) {
            // Start drag operation for cross reference link
            if (!draggingInProgress) {
                draggingInProgress = true;
                var index = listView.indexAt( mousePressedX, mousePressedY + listView.contentY);
                btQmlInterface.dragHandler(index, pressedLink);
            }
        } else {
            // Do text selection
            selectedTextColumn = Math.floor(mousePressedX / (listView.width / listView.columns));
            getSelectedTextPositions(...sortMousePoints(x, y));
            selectByItem();
            selectionInProgress = true;
        }
    }

    function leftMouseRelease(x, y) {
        mousePressedAndMoving = false;
        processMouseRelease(x, y);
        draggingInProgress = false;
        selectionInProgress = false;
    }

    function processMouseRelease(x, y) {
        if (draggingInProgress)
            return;
        if (selectionInProgress)
            return;
        deselectByItem();
        if (openPersonalCommentary(mousePressedX, mousePressedY))
            return;
        if (isBibleReference(pressedLink))
            btQmlInterface.setBibleKey(pressedLink);
    }

    function isBibleReference(url) {
        if (url === "" || url.includes("lemmamorph") || url.includes("footnote"))
            return false;
        return true;
    }

    function sortMousePoints(mouseMovedX, mouseMovedY) {
        if (mousePressedX < mouseMovedX) {
            if (mousePressedY < mouseMovedY) {
                return [Qt.point(mousePressedX, mousePressedY),
                        Qt.point(mouseMovedX, mouseMovedY)];
            } else {
                return [Qt.point(mousePressedX, mouseMovedY),
                        Qt.point(mouseMovedX, mousePressedY)];
            }
        } else {
            if (mousePressedY < mouseMovedY) {
                return [Qt.point(mouseMovedX, mousePressedY),
                        Qt.point(mousePressedX, mouseMovedY)];
            } else {
                return [Qt.point(mouseMovedX, mouseMovedY),
                        Qt.point(mousePressedX, mousePressedY)];
            }
        }
    }

    function getSelectedTextPositions(mouseUL, mouseLR) {
        const firstDelegateItemIndex = listView.indexAt(mouseUL.x, mouseUL.y + listView.contentY);
        const lastDelegateItemIndex = listView.indexAt(mouseLR.x, mouseLR.y + listView.contentY);
        if ((lastDelegateItemIndex < 0) || (lastDelegateItemIndex < 0))
            return;

        indexFirst = firstDelegateItemIndex;
        itemFirst = listView.itemAtIndex(indexFirst);
        const delegateXFirst = listView.contentX + mouseUL.x - itemFirst.x;
        const delegateYFirst = listView.contentY + mouseUL.y - itemFirst.y;
        textPosFirst = itemFirst.positionAt(delegateXFirst, delegateYFirst, selectedTextColumn);

        indexLast = lastDelegateItemIndex;
        itemLast = listView.itemAtIndex(indexLast);
        const delegateXLast = listView.contentX + mouseLR.x - itemLast.x;
        const delegateYLast = listView.contentY + mouseLR.y - itemLast.y;
        textPosLast = itemLast.positionAt(delegateXLast, delegateYLast, selectedTextColumn);
    }

    function selectDelegateItem(index, dItem) {
        if (dItem === null || dItem === 0)
            return;
        if (dItem === itemFirst && dItem === itemLast)
            dItem.selectSingle(selectedTextColumn, textPosFirst, textPosLast);
        else if (dItem === itemFirst)
            dItem.selectFirst(selectedTextColumn, textPosFirst);
        else if (dItem === itemLast)
            dItem.selectLast(selectedTextColumn, textPosLast);
        else if (dItem !== itemFirst && dItem !== itemLast)
            dItem.selectAll(selectedTextColumn);
        else
            return;

        var columnSelectedText = dItem.getSelectedText(selectedTextColumn);
        btQmlInterface.saveSelectedText(index, columnSelectedText)
    }

    function nextItem(currentItem) {
        var nextY = currentItem.y+currentItem.height+listView.spacing;
        var nextI = listView.itemAt(currentItem.x, nextY);
        return nextI;
    }

    function selectByItem() {
        var delegateItem = itemFirst;
        var loopIndex = 0;
        while (true) {
            selectDelegateItem(loopIndex, delegateItem);
            if (delegateItem == itemLast)
                break;
            delegateItem = nextItem(delegateItem);
            loopIndex++;
        }
    }

    function deselectByItem() {
        if (itemFirst == null)
            return;
        var delegateItem = itemFirst;
        while (true) {
            delegateItem.deselect(selectedTextColumn);
            if (delegateItem == itemLast)
                break;
            delegateItem = nextItem(delegateItem);
        }
        itemFirst = null;
        itemLast = null
        selectedTextColumn = 0;
        btQmlInterface.clearSelectedText();
    }

    function openPersonalCommentary(x, y) {
        var index = listView.indexAt( x,y + listView.contentY);
        var column = Math.floor(x / (listView.width / listView.columns));
        return listView.startEdit(index, column);
    }

    function updateReferenceText() {
        listView.updateReferenceText();
    }

    function scroll(value) {
        listView.scroll(value);
    }

    function isBlank(text) {
        var t = text;
        t.trim();
        return t.length === 0;
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
        property int backgroundHighlightIndex: btQmlInterface.backgroundHighlightColorIndex

        function scroll(value) {
            var y = contentY;
            contentY = y + value;
        }

        function startEdit(row, column) {
            if (!btQmlInterface.moduleIsWritable(column))
                return false;
            if (btQmlInterface.indexToVerse(row) === 0 )
                return false;
            savedRow = row;
            savedColumn = column;
            btQmlInterface.openEditor(row, column);
            return true;
        }

        function finishEdit(newText) {
            btQmlInterface.setRawText(savedRow, savedColumn, newText)
        }

        function updateReferenceText() {
            var index = indexAt(contentX,contentY+30);
            if (index < 0)
                return;
            btQmlInterface.changeReference(index);
        }

        clip: true
        anchors.fill: parent
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.topMargin: 0
        anchors.bottomMargin: 0
        boundsMovement: Flickable.StopAtBounds
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

        delegate: DisplayDelegate {
            id: dd
//            Component.onCompleted: {
//                selectDelegateItem(index, dd);
//            }
        }
    }
}
