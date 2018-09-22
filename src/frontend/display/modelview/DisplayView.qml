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

    function saveContextMenuIndex(x, y) {
        contextMenuIndex = displayListView.indexAt(x,y+displayListView.contentY);
    }

    function moveContentLocation(value) {
        displayListView.scroll(value);
    }

    function updateReferenceText() {
        displayListView.updateReferenceText();
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
    }

    ListView {
        id: displayListView

        property color textColor: "black"
        property color textBackgroundColor: "white"

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
            var rawText = btQmlInterface.getRawText(row, column);
            textEditor.editFinished.connect( displayListView.finishEdit);
            textEditor.open(rawText);
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
        maximumFlickVelocity: 750
        model: btQmlInterface.textModel
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

                Rectangle {
                    id: space1
                    height: 10
                    width: parent.spacing
                    anchors.left: parent.left
                    anchors.top: parent.top
                }

                Text {
                    id: column0Text
                    textFormat: Text.RichText
                    text: text1
                    anchors.top: parent.top
                    anchors.left: space1.right
                    width: parent.textWidth
                    color: displayListView.textColor
                    font.family: btQmlInterface.fontName0
                    font.pointSize: btQmlInterface.fontSize0
                    wrapMode: Text.WordWrap
                    visible: displayListView.columns > 0
                    onWidthChanged: doLayout()
                    onLinkHovered: delegate.hovered(link)

                    MouseArea {
                        id: mouseArea0
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        drag.target: space1
                        drag.onActiveChanged: delegate.dragStart(index, mouseArea0.drag.active)
                    }
                }

                Rectangle {
                    id: space2
                    height: 10
                    width: parent.spacing
                    anchors.left: column0Text.right
                    anchors.top: parent.top
                }

                Text {
                    id: column1Text
                    text: text2
                    textFormat: Text.RichText
                    anchors.top: parent.top
                    anchors.left: space2.right
                    anchors.leftMargin: 0
                    width: parent.textWidth
                    color: displayListView.textColor
                    font.family: btQmlInterface.fontName1
                    font.pointSize: btQmlInterface.fontSize1
                    wrapMode: Text.WordWrap
                    visible: displayListView.columns > 1
                    onWidthChanged: doLayout()
                    onLinkHovered: delegate.hovered(link)

                    MouseArea {
                        id: mouseArea1
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        drag.target: space1
                        drag.onActiveChanged: delegate.dragStart(index, mouseArea1.drag.active)
                    }
                }

                Rectangle {
                    id: space3
                    height: 10
                    width: parent.spacing
                    anchors.left: column1Text.right
                    anchors.top: parent.top
                }

                Text {
                    id: column2Text
                    text: text3
                    textFormat: Text.RichText
                    anchors.top: parent.top
                    anchors.left: space3.right
                    anchors.leftMargin: 0
                    width: parent.textWidth
                    color: displayListView.textColor
                    font.family: btQmlInterface.fontName2
                    font.pointSize: btQmlInterface.fontSize2
                    wrapMode: Text.WordWrap
                    visible: displayListView.columns > 2
                    onWidthChanged: doLayout()
                    onLinkHovered: delegate.hovered(link)

                    MouseArea {
                        id: mouseArea2
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        drag.target: space1
                        drag.onActiveChanged: delegate.dragStart(index, mouseArea2.drag.active)
                    }
                }

                Rectangle {
                    id: space4
                    height: 10
                    width: parent.spacing
                    anchors.left: column2Text.right
                    anchors.top: parent.top
                }

                Text {
                    id: column3Text
                    text: text4
                    textFormat: Text.RichText
                    anchors.top: parent.top
                    anchors.left: space4.right
                    anchors.leftMargin: 0
                    width: parent.textWidth
                    color: displayListView.textColor
                    font.family: btQmlInterface.fontName3
                    font.pointSize: btQmlInterface.fontSize3
                    wrapMode: Text.WordWrap
                    visible: displayListView.columns > 3
                    onWidthChanged: doLayout()

                    onLinkHovered: delegate.hovered(link)

                    MouseArea {
                        id: mouseArea3
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        drag.target: space1
                        drag.onActiveChanged: delegate.dragStart(index, mouseArea3.drag.active)
                    }
                }
            }
        }
    }
}

