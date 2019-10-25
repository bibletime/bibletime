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

import BibleTime 1.0
import QtQuick 2.2

Rectangle {
    id: display

    property int contextMenuIndex: btQmlInterface.contextMenuIndex
    property int leftMousePressIndex: -1
    property int leftMousePressX: 0
    property int leftMousePressY: 0
    property int leftMouseReleaseIndex: -1
    property int dragDistance: 8

    function saveContextMenuIndex(x, y) {
        contextMenuIndex = displayListView.indexAt(x,y+displayListView.contentY);
    }

    function leftMouseMove(x, y) {
        if (leftMousePressIndex < 0)
            return;
        if ((Math.abs(leftMousePressX - x) < dragDistance) && (Math.abs(leftMousePressY - y) < dragDistance)) {
            return;
        }
        var moveIndex = displayListView.indexAt(x,y+displayListView.contentY);
        if (moveIndex < 0)
            return;
        var column = Math.floor(x / (displayListView.width / displayListView.columns));
        btQmlInterface.selectByIndex(leftMousePressIndex, moveIndex, column);
    }

    function leftMousePress(x, y) {
        var item1 = displayListView.itemAt( x,y + displayListView.contentY);
        var xItem = x - item1.x + displayListView.contentX;
        var yItem = y - item1.y + displayListView.contentY;
        var  url = item1.linkAt(xItem, yItem);

        if (url === "" || url.includes("lemmamorph") || url.includes("footnote")) {
            // Start selection of items
            leftMousePressX = x;
            leftMousePressY = y;
            leftMousePressIndex = displayListView.indexAt(x,y+displayListView.contentY);
            leftMouseReleaseIndex = leftMousePressIndex;
            return;
        }

        // Start drag operation for link
        var index = displayListView.indexAt( x,y + displayListView.contentY);
        btQmlInterface.dragHandler(index, url);
        return;
    }

    function leftMouseRelease(x, y) {
        if (leftMousePressIndex < 0)
            return;
        if ((Math.abs(leftMousePressX - x) < dragDistance) && (Math.abs(leftMousePressY - y) < dragDistance)) {
            leftMousePressIndex = -1;
            if (btQmlInterface.isSelected())
                btQmlInterface.deSelect();
            else {
                openPersonalCommentary(leftMousePressX, leftMousePressY);
            }
            return;
        }
        leftMouseReleaseIndex = displayListView.indexAt(x,y+displayListView.contentY);
        var moveIndex = displayListView.indexAt(x,y+displayListView.contentY);
        if (moveIndex < 0)
            return;
        var column = Math.floor(x / (displayListView.width / displayListView.columns));
        btQmlInterface.selectByIndex(leftMousePressIndex, moveIndex, column);
    }

    function openPersonalCommentary(x, y) {
        var index = displayListView.indexAt( x,y + displayListView.contentY);
        var column = Math.floor(x / (displayListView.width / displayListView.columns));
        displayListView.startEdit(index, column);
    }

    function updateReferenceText() {
        displayListView.updateReferenceText();
    }

    function scroll(value) {
        displayListView.scroll(value);
    }

    width: 10
    height: 10
    color: "white"

    BtQmlInterface {
        id: btQmlInterface

        onPageDownChanged: {
            displayListView.scroll(displayListView.height * 0.8);
            updateReferenceText();
        }
        onPageUpChanged: {
            displayListView.scroll(displayListView.height * -0.8);
            updateReferenceText();
        }
        onPositionItemOnScreen: {
            displayListView.positionViewAtIndex(index, ListView.Contain);
            updateReferenceText();
        }
    }


    ListView {
        id: displayListView

        property color textColor: "black"
        property color textBackgroundColor: "white"
        property color selectedBackgroundColor: "#000070"

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
                property int spacing: 2.5 * btQmlInterface.pixelsPerMM
                property int textWidth: (displayListView.width / displayListView.columns) - (spacing *  ((displayListView.columns+1)/displayListView.columns)  )
                property int vertSpace: 2 * btQmlInterface.pixelsPerMM

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
                    btQmlInterface.setMagReferenceByUrl(link);
                    btQmlInterface.activeLink = link;
                }

                function dragStart(index, active) {
                    if (active) {
                        btQmlInterface.dragHandler(index, btQmlInterface.activeLink);
                    }
                }

                color: displayListView.textBackgroundColor
                width: displayListView.width
                height: {
                    if (displayListView.columns == 1)
                        return Math.max(column0Text.height,20) + vertSpace
                    if (displayListView.columns == 2)
                        return Math.max(column0Text.height, column1Text.height) + vertSpace
                    if (displayListView.columns == 3)
                        return Math.max(column0Text.height, column1Text.height, column2Text.height) + vertSpace
                    if (displayListView.columns == 4)
                        return Math.max(column0Text.height, column1Text.height,
                                        column2Text.height, column3Text.height) + vertSpace
                    return 30;
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
                    color: (selected && columnSelected === 0) ? displayListView.selectedBackgroundColor : displayListView.textBackgroundColor
                }

                TextEdit {
                    id: column0Text
                    textFormat: Text.RichText
                    readOnly: true
                    text: text1
                    anchors.top: parent.top
                    anchors.left: space1.right
                    width: parent.textWidth
                    color: (selected && columnSelected === 0) ? "white" : displayListView.textColor
                    font.family: btQmlInterface.fontName0
                    font.pointSize: btQmlInterface.fontSize0
                    wrapMode: Text.WordWrap
                    visible: displayListView.columns > 0
                    z:1
                    onLinkHovered: delegate.hovered(link)
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
                    color: (selected && columnSelected === 1) ? displayListView.selectedBackgroundColor : displayListView.textBackgroundColor
                }

                TextEdit {
                    id: column1Text
                    text: text2
                    textFormat: Text.RichText
                    anchors.top: parent.top
                    anchors.left: space2.right
                    anchors.leftMargin: 0
                    readOnly: true
                    width: parent.textWidth
                    color: (selected && columnSelected === 1) ? "white" : displayListView.textColor
                    font.family: btQmlInterface.fontName1
                    font.pointSize: btQmlInterface.fontSize1
                    wrapMode: Text.WordWrap
                    visible: displayListView.columns > 1
                    onLinkHovered: delegate.hovered(link)
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
                    color: (selected && columnSelected === 2) ? displayListView.selectedBackgroundColor : displayListView.textBackgroundColor
                }

                TextEdit {
                    id: column2Text
                    text: text3
                    textFormat: Text.RichText
                    anchors.top: parent.top
                    anchors.left: space3.right
                    anchors.leftMargin: 0
                    readOnly: true
                    width: parent.textWidth
                    color: (selected && columnSelected === 2) ? "white" : displayListView.textColor
                    font.family: btQmlInterface.fontName2
                    font.pointSize: btQmlInterface.fontSize2
                    wrapMode: Text.WordWrap
                    visible: displayListView.columns > 2
                    onLinkHovered: delegate.hovered(link)
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
                    color: (selected && columnSelected === 3) ? displayListView.selectedBackgroundColor : displayListView.textBackgroundColor
                }

                TextEdit {
                    id: column3Text
                    text: text4
                    textFormat: Text.RichText
                    anchors.top: parent.top
                    anchors.left: space4.right
                    anchors.leftMargin: 0
                    readOnly: true
                    width: parent.textWidth
                    color: (selected && columnSelected === 3) ? "white" : displayListView.textColor
                    font.family: btQmlInterface.fontName3
                    font.pointSize: btQmlInterface.fontSize3
                    wrapMode: Text.WordWrap
                    visible: displayListView.columns > 3
                    onLinkHovered: delegate.hovered(link)
                }
            }
        }
    }
}


