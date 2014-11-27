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

import QtQuick 2.2
import BibleTime 1.0

Rectangle {
    id: windowView

    property string title: toolbar.title

    signal swipeLeft
    signal swipeRight

    function getModuleLanguage() {
        return btWindowInterface.moduleLanguage
    }

    function updateTextFonts() {
        btWindowInterface.updateTextFonts();
    }

    function getModule() {
        return btWindowInterface.moduleName;
    }

    function getReference() {
        return btWindowInterface.reference;
    }

    function setModule(module) {
        btWindowInterface.moduleName = module;
    }

    function setKey(key) {
        btWindowInterface.reference = key;
        btWindowInterface.updateCurrentModelIndex();
    }

    function saveWindowStateToConfig (index) {
        btWindowInterface.saveWindowStateToConfig(index);
    }

    function setHistoryPoint() {
        btWindowInterface.setHistoryPoint();
    }

    function setModuleToBeginning() {
        btWindowInterface.setModuleToBeginning();
    }

    color: "black"

    BtWindowInterface {
        id: btWindowInterface
    }

    BtStyle {
        id:btStyle
    }

    Rectangle {
        id: toolbar

        property string title: btWindowInterface.moduleName + " (" + btWindowInterface.reference + ")"

        width: parent.width
        height: {
            var pixel = btStyle.pixelsPerMillimeterY * 8;
            var uiFont = btStyle.uiFontPointSize * 4.4;
            var mix = pixel * 0.7 + uiFont * 0.3;
            return Math.max(pixel, mix);
        }


        color: btStyle.toolbarColor
        border.width: 1
        border.color: btStyle.toolbarColor

        PrevNextArrow {
            id: prevHistory

            background: btStyle.toolbarColor
            textColor: btStyle.toolbarTextColor
            borderColor: btStyle.toolbarTextColor
            prev: true
            width: parent.height * 0.80
            height: parent.height * 0.80
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: parent.height * 0.1;
            show: btWindowInterface.historyBackwardVisible
            onClicked: {
                if (show)
                    btWindowInterface.moveHistoryBackward();
            }
        }

        PrevNextArrow {
            id: nextHistory

            background: btStyle.toolbarColor
            textColor: btStyle.toolbarTextColor
            borderColor: btStyle.toolbarTextColor
            prev: false
            width: parent.height * 0.80
            height: parent.height * 0.80
            anchors.left: prevHistory.right
            anchors.top: parent.top
            anchors.margins: parent.height * 0.1;
            show: btWindowInterface.historyForwardVisible
            onClicked: {
                if (show)
                    btWindowInterface.moveHistoryForward();
            }
        }

        Rectangle {
            id: moduleDisplay

            width: text.width + 30
            radius:btStyle.pixelsPerMillimeterX
            anchors.left: nextHistory.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: btStyle.pixelsPerMillimeterY * 0.7
            anchors.leftMargin: parent.height * 0.1
            anchors.bottomMargin: btStyle.pixelsPerMillimeterY * 0.7
            color: btStyle.textBackgroundColor
            border.color: btStyle.toolbarTextColor
            border.width: 2

            Text {
                id: text

                anchors.centerIn: parent
                anchors.leftMargin: 4
                anchors.rightMargin: 4
                font.pointSize: btStyle.uiFontPointSize
                elide: Text.ElideMiddle
                color: btStyle.textColor
                text: btWindowInterface.moduleName
            }

            MouseArea {
                id: moduleMouseArea

                anchors.fill: parent
                onClicked: {
                    btWindowInterface.changeModule();
                }
            }
        }


        Rectangle {
            id: referenceDisplay

            width: {
                var w2 = toolbar.width - prevHistory.width -prevHistory.anchors.margins*2;
                w2 = w2 - nextHistory.width -nextHistory.anchors.margins*2;
                w2 = w2 - moduleDisplay.width - moduleDisplay.anchors.leftMargin - moduleDisplay.anchors.rightMargin;
                w2 - w2 - parent.height * 0.2;
                return w2;
            }
            radius: btStyle.pixelsPerMillimeterX
            anchors.left: moduleDisplay.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: btStyle.pixelsPerMillimeterY * 0.7
            anchors.bottomMargin: btStyle.pixelsPerMillimeterY * 0.7
            anchors.leftMargin:  parent.height * 0.1
            color: btStyle.textBackgroundColor
            border.color: btStyle.toolbarTextColor
            border.width: 2

            Text {
                id: referenceText
                anchors.fill: parent
                anchors.leftMargin: btStyle.pixelsPerMillimeterX *4.5
                anchors.rightMargin: 4
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.pointSize: btStyle.uiFontPointSize
                elide: Text.ElideMiddle
                color: btStyle.textColor
                text: btWindowInterface.reference
            }

            MouseArea {
                id: mouseArea

                anchors.fill: parent
                onClicked: {
                    btWindowInterface.changeReference();
                }
            }
        }

    }

    Rectangle {
        id: mainTextView

        color: btStyle.textBackgroundColor
        anchors.top: toolbar.bottom
        anchors.left: windowView.left
        anchors.right: windowView.right
        anchors.bottom: windowView.bottom

        ListView {
            id: listView

            clip: true
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            model: btWindowInterface.textModel
            highlightFollowsCurrentItem: true
            currentIndex: btWindowInterface.currentModelIndex
            onCurrentIndexChanged: {
                positionViewAtIndex(currentIndex,ListView.Beginning)
            }
            onMovementEnded: {
                var index = indexAt(contentX,contentY+30);
                btWindowInterface.updateKeyText(index);
            }

            delegate: Text {
                text: line
                textFormat: Text.RichText
                width: parent.width
                color: btStyle.textColor
                font.family: btWindowInterface.fontName
                font.pointSize: btWindowInterface.fontSize
                wrapMode: Text.WordWrap
                onWidthChanged: doLayout()
            }

            MultiPointTouchArea {
                id: globalTouchArea

                property bool activeGesture: false
                property int firstX
                property int firstY

                anchors.fill: parent
                enabled: false
                minimumTouchPoints: 1
                maximumTouchPoints: 1
                z:0
                touchPoints: [
                    TouchPoint { id: touch1 }
                ]

                onPressed: {
                    firstX = touch1.x
                    firstY = touch1.y
                }

                onGestureStarted: {
                    if (!activeGesture) {
                        var dx = touch1.x - firstX
                        var dy = touch1.y - firstY
                        if (Math.abs(dx) > dy * 3  &&  Math.abs(dx) > 0.05 * width) {
                            activeGesture = true;
                            gesture.grab()
                        }
                    }
                }

                onReleased: {
                    if (activeGesture) {
                        var dx = touch1.x - firstX;
                        var dy = touch1.y - firstY;
                        if (dx < 0) {
                            windowView.swipeLeft()
                        }
                        else {
                            windowView.swipeRight()
                        }
                    }
                    activeGesture = false;
                }
            }
        }
    }
}
