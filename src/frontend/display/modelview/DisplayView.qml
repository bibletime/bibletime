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
    id: displayView

    // Mouse movement properties
    property bool mousePressedAndMoving: false
    property int mousePressedX: 0
    property int mousePressedY: 0
    property int mousePressedColumnIndex: -1
    property int mousePressedItemIndex: -1
    property int mousePressedTextPosition: -1
    property bool draggingInProgress: false
    property bool selectionInProgress: false
    property string pressedLink: ""

    // Text selection properties
    property var selectionStart: null
    property var selectionEnd: null

    function textIsHtml(dtext) {
        if (dtext.includes("<!DOCTYPE") || dtext.includes("<span"))
            return Text.RichText;
        return Text.PlainText;
    }

    function deselectCurrentSelection() {
        if (selectionStart === null || selectionEnd === null)
            return;
        let [start, end] = [selectionStart.itemIndex, selectionEnd.itemIndex];
        if (end < start)
            [start, end] = [end, start];
        do {
            const item = listView.itemAtIndex(start);
            if (item !== null) {
                /// \todo support multi-column selections:
                item.deselect(selectionStart.columnIndex);
            }
        } while (++start <= end);
    }

    function leftMousePress(x, y) {
        mousePressedX = x;
        mousePressedY = y;
        mousePressedColumnIndex =
                Math.floor(x / (listView.width / listView.columns));
        const cx = x + listView.contentX;
        const cy = y + listView.contentY;
        mousePressedItemIndex = listView.indexAt(cx, cy);
        if (mousePressedItemIndex < 0) {
            mousePressedTextPosition = -1;
        } else {
            const pressedItem = listView.itemAtIndex(mousePressedItemIndex);
            const rx = cx - pressedItem.x;
            const ry = cy - pressedItem.y;
            mousePressedTextPosition =
                    pressedItem.positionAt(rx, ry, mousePressedColumnIndex);
            pressedLink = pressedItem.linkAt(rx, ry);
        }
    }

    function leftMouseMove(x, y) {
        // Do nothing unless mouse moved some distance from pressed location
        if (!mousePressedAndMoving) {
            const dragDistance = 8;
            if ((Math.abs(mousePressedX - x) < dragDistance)
                && (Math.abs(mousePressedY - y) < dragDistance))
                return;
            mousePressedAndMoving = true;
        }

        /// \todo Handle cases of previous selections before left mouse press?

        // Handle dragging of cross references:
        if (draggingInProgress)
            return; // Prevent starting of selection and/or new drags:
        if (!selectionInProgress && isBibleReference(pressedLink)) {
            deselectCurrentSelection();
            selectionStart = null;
            selectionEnd = null;
            BtQmlInterface.clearSelection();

            draggingInProgress = true;
            BtQmlInterface.dragHandler(mousePressedItemIndex, pressedLink);
            return; // Prevent starting of selection
        }

        if (!selectionInProgress) {
            if (mousePressedTextPosition < 0) // Does not start selection
                return;
            deselectCurrentSelection(); /// \todo This can be optimized
            BtQmlInterface.clearSelection();
            selectionInProgress = true;
            selectionStart = {
                columnIndex: mousePressedColumnIndex,
                itemIndex: mousePressedItemIndex,
                textPosition: mousePressedTextPosition,
            };
        } else {
            deselectCurrentSelection(); /// \todo This can be optimized
        }

        const cx = x + listView.contentX;
        const cy = y + listView.contentY;
        const endItemIndex = listView.indexAt(cx, cy);
        if (endItemIndex < 0)
            return; // Don't change current selection

        const currentItem = listView.itemAtIndex(endItemIndex);
        const rx = cx - currentItem.x;
        const ry = cy - currentItem.y;
        const endColumnIndex = selectionStart.columnIndex; /// \todo multi-col
        selectionEnd = {
            columnIndex: endColumnIndex,
            itemIndex: endItemIndex,
            textPosition: currentItem.positionAt(rx, ry, endColumnIndex),
        };

        // Apply selection
        let [start, end] = [selectionStart, selectionEnd];
        if (end.itemIndex < start.itemIndex) {
            [start, end] = [end, start];
        } else if (end.itemIndex == start.itemIndex) { // Single-row selection
            if (end.textPosition < start.textPosition)
                [start, end] = [end, start];
            currentItem.selectSingle(start.columnIndex,
                                     start.textPosition,
                                     end.textPosition);
            return;
        }
        {
            const startItem = listView.itemAtIndex(start.itemIndex);
            if (startItem !== null)
                startItem.selectFirst(start.columnIndex, start.textPosition);
        }
        for (let i = start.itemIndex + 1; i < end.itemIndex; ++i) {
            const item = listView.itemAtIndex(i);
            if (item !== null)
                item.selectAll(start.columnIndex);
        }
        {
            const endItem = listView.itemAtIndex(end.itemIndex);
            if (endItem !== null)
                endItem.selectLast(end.columnIndex, end.textPosition);
        }
    }

    // Since the itemAtIndex() function of the ListView will return null for
    // items which are not in view or cached, we can't use it to calculate the
    // whole of the selected text. Hence we just recreate the respective
    // TextView QML widgets here to retrieve the selected texts.
    function selectedFromTextEdit(row, column, selectCallback) {
        const rawText = BtQmlInterface.rawText(row, column);
        const item = Qt.createQmlObject(
                'import QtQuick 2.10; TextEdit { visible: false }',
                displayView); // parent
        try {
            item.textFormat = textIsHtml(rawText);
            item.text = rawText;
            selectCallback(item);
            return item.selectedText;
        } finally {
            item.destroy();
        }
    }

    function leftMouseRelease(x, y) {
        mousePressedAndMoving = false;
        if (draggingInProgress) {
            draggingInProgress = false;
            return;
        }

        if (selectionInProgress) { // Complete selection:
            selectionInProgress = false;

            let [start, end] = [selectionStart, selectionEnd];
            if (end.itemIndex < start.itemIndex) {
                [start, end] = [end, start];
            } else if (end.itemIndex == start.itemIndex) { // Single row
                if (end.textPosition < start.textPosition) {
                    [start, end] = [end, start];
                } else if (end.textPosition == start.textPosition) {
                    BtQmlInterface.clearSelection();
                    return;
                }
                BtQmlInterface.setSelection(
                        start.columnIndex,
                        start.itemIndex,
                        start.itemIndex,
                        selectedFromTextEdit(
                            start.itemIndex,
                            start.columnIndex,
                            (item) => item.select(start.textPosition,
                                                  end.textPosition)));
                return;
            }

            // Reconstruct selected text from multiple item indexes:
            const selectedTexts = [
                selectedFromTextEdit(
                        start.itemIndex,
                        start.columnIndex,
                        (item) => item.select(start.textPosition, item.length)),
            ];
            for (let i = start.itemIndex + 1; i < end.itemIndex; ++i)
                selectedTexts.push(
                        selectedFromTextEdit(
                            i, start.columnIndex, (item) => item.selectAll()));
            selectedTexts.push(
                    selectedFromTextEdit(
                        end.itemIndex,
                        start.columnIndex,
                        (item) => item.select(0, end.textPosition)));
            BtQmlInterface.setSelection(
                    start.columnIndex,
                    start.itemIndex,
                    end.itemIndex,
                    selectedTexts.join("\n"));
            return;
        }

        deselectCurrentSelection();
        selectionStart = null;
        selectionEnd = null;
        BtQmlInterface.clearSelection();

        if (openPersonalCommentary(mousePressedX, mousePressedY))
            return;
        if (isBibleReference(pressedLink))
            BtQmlInterface.setBibleKey(pressedLink);
    }

    function isBibleReference(url) {
        if (url === "" || url.includes("lemmamorph") || url.includes("footnote"))
            return false;
        return true;
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

    function pageDown() {
        listView.scroll(listView.height * 0.8);
        updateReferenceText();
    }

    function pageUp() {
        listView.scroll(listView.height * -0.8);
        updateReferenceText();
    }

    function handlePositionItemOnScreen() {
        listView.positionViewAtIndex(index, ListView.Contain);
        updateReferenceText();
    }

    Component.onCompleted: {
        BtQmlInterface.positionItemOnScreen.connect(handlePositionItemOnScreen);
    }

    width: 10
    height: 10
    color: BtQmlInterface.backgroundColor

    ListView {
        id: listView

        property color textColor: BtQmlInterface.foregroundColor
        property color textBackgroundColor: BtQmlInterface.backgroundColor
        property int columns: BtQmlInterface.numModules
        property int savedRow: 0
        property int savedColumn: 0
        property int backgroundHighlightIndex: BtQmlInterface.backgroundHighlightColorIndex

        function scroll(value) {
            var y = contentY;
            contentY = y + value;
        }

        function startEdit(row, column) {
            if (!BtQmlInterface.moduleIsWritable(column))
                return false;
            if (BtQmlInterface.indexToVerse(row) === 0 )
                return false;
            savedRow = row;
            savedColumn = column;
            BtQmlInterface.openEditor(row, column);
            return true;
        }

        function finishEdit(newText) {
            BtQmlInterface.setRawText(savedRow, savedColumn, newText)
        }

        function updateReferenceText() {
            var index = indexAt(contentX,contentY+30);
            if (index < 0)
                return;
            BtQmlInterface.changeReference(index);
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
        model: BtQmlInterface.textModel
        spacing: 2
        highlightFollowsCurrentItem: true
        currentIndex: BtQmlInterface.currentModelIndex
        onCurrentIndexChanged: {
            positionViewAtIndex(currentIndex,ListView.Beginning)
        }
        onMovementEnded: {
            updateReferenceText();
        }

        delegate: DisplayDelegate {
            // Due to the delegates being destroyed and re-created when the
            // ListView is scrolled or flicked, we need to re-apply the
            // any selection to the newly recreated delegates.
            Component.onCompleted: {
                if (selectionStart === null || selectionEnd === null)
                    return;
                /// \todo support multi-column selections:
                if (selectionStart.columnIndex >= listView.columns)
                    return;
                const columnItem = getColumnItem(selectionStart.columnIndex);
                let [start, end] = [selectionStart, selectionEnd];
                if (end.itemIndex < start.itemIndex) {
                    [start, end] = [end, start];
                } else if (end.itemIndex == start.itemIndex) {
                    if (end.textPosition < start.textPosition) {
                        [start, end] = [end, start];
                    } else if (end.textPosition == start.textPosition) {
                        return;
                    }
                    if (index == start.itemIndex)
                        columnItem.select(start.textPosition, end.textPosition);
                    return;
                }

                if ((index < start.itemIndex) || (index > end.itemIndex))
                    return;
                if (index == start.itemIndex) {
                    columnItem.select(start.textPosition, columnItem.length);
                } else if (index == end.itemIndex) {
                    columnItem.select(0, end.textPosition);
                } else {
                    columnItem.selectAll();
                }
            }
        }
    }
}
