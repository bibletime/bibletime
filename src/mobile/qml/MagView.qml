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

import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import BibleTime 1.0

FocusScope {
    id: magView

    property int magViewMargins: btStyle.pixelsPerMillimeterX

    signal magFinished();

    function setModule(module) {
        btWindowInterface.moduleName = module;
        btWindowInterface2.moduleName = module;
    }

    function setReference(reference) {
        btWindowInterface.reference = reference;
    }

    function updateTextFont() {
        btWindowInterface.updateTextFonts();
        btWindowInterface2.updateTextFonts();
    }

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && magView.focus == true) {
            magFinished();
            event.accepted = true;
        }
    }

    BtWindowInterface {
        id: btWindowInterface

        isMagView: true
        onModuleChanged: {
            btWindowInterface2.moduleName = btWindowInterface.moduleName
        }
    }

    BtWindowInterface {
        id: btWindowInterface2

        onReferenceChange: {
            btWindowInterface.updateCurrentModelIndex();
        }
    }

    Rectangle {
        id: magArea

        property real topBottomSplit: 0.5

        anchors.fill: parent
        color: btStyle.textBackgroundColor

        Rectangle {
            id: toolbar

            height: {
                var pixel = btStyle.pixelsPerMillimeterY * 7;
                var uiFont = btStyle.uiFontPointSize * 3;
                var mix = pixel * 0.7 + uiFont * 0.3;
                return Math.max(pixel, mix);
            }
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            color: btStyle.toolbarColor

            Rectangle {
                id: moduleDisplay

                width: text.width + 30
                radius:btStyle.pixelsPerMillimeterX
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.leftMargin: magView.magViewMargins
                anchors.topMargin: 2
                anchors.bottomMargin: 2
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
                    var w2 = toolbar.width - moduleDisplay.width - toolbar.height * 2
                    return w2;
                }
                anchors.left: moduleDisplay.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.topMargin: 2
                anchors.bottomMargin: 2
                anchors.leftMargin:  magView.magViewMargins
                border.color: btStyle.toolbarTextColor
                border.width: 2
                color: btStyle.textBackgroundColor
                radius: btStyle.pixelsPerMillimeterX

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

        SplitView {
            id: topSplitter

            orientation: Qt.Vertical
            anchors.top: toolbar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            handleDelegate: Rectangle {
                id: handle

                height: toolbar.height
                color: btStyle.textBackgroundColor

                Rectangle {
                    id: handleBar

                    anchors.fill: parent
                    anchors.leftMargin: magView.magViewMargins
                    anchors.rightMargin: magView.magViewMargins
                    radius:btStyle.pixelsPerMillimeterX
                    color: btStyle.toolbarColor
                    border.color: btStyle.toolbarTextColor
                    border.width: 2

                    Text {
                        id: referenceTitle

                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        width: parent.width * .95
                        anchors.leftMargin: parent.height * 0.25
                        font.pointSize: btStyle.uiFontPointSize
                        color: btStyle.textColor
                        elide: Text.ElideMiddle
                        text: btWindowInterface2.referencesViewTitle
                    }
                }
            }

            Rectangle {
                id: documentView

                color: btStyle.textBackgroundColor
                width: parent.width
                height: magArea.height * magArea.topBottomSplit

                ListView {
                    id: documentListView

                    clip: true
                    anchors.fill: parent
                    anchors.leftMargin: magView.magViewMargins
                    anchors.rightMargin: magView.magViewMargins
                    anchors.topMargin: magView.magViewMargins
                    anchors.bottomMargin: magView.magViewMargins
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
                        onLinkActivated: {
                            btWindowInterface2.setReferenceByUrl(link);
                            btWindowInterface2.updateCurrentModelIndex();
                        }
                    }
                }
            }

            Rectangle {
                id: referencesView

                property int refMargins: btStyle.pixelsPerMillimeterX * 2
                color: btStyle.textBackgroundColor
                width: parent.width

                ScrollView {
                    id: footnoteView

                    anchors.fill: parent
                    anchors.leftMargin: magView.magViewMargins
                    anchors.rightMargin: magView.magViewMargins
                    anchors.topMargin: magView.magViewMargins
                    anchors.bottomMargin: magView.magViewMargins
                    horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
                    verticalScrollBarPolicy: Qt.ScrollBarAlwaysOff
                    visible: btWindowInterface2.footnoteVisible
                    z: 1
                    Text {
                        id: footnoteText

                        color: btStyle.textColor
                        font.family: btWindowInterface2.fontName
                        font.pointSize: btWindowInterface2.fontSize
                        text: btWindowInterface2.footnoteText
                        textFormat: Text.RichText
                        width: footnoteView.width
                        wrapMode: Text.WordWrap
                    }
                }

                ListView {
                    id: referencesListView

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.top: parent.top
                    clip: true
                    width: parent.width
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8
                    model: btWindowInterface2.textModel
                    highlightFollowsCurrentItem: true
                    currentIndex: btWindowInterface2.currentModelIndex
                    visible: !btWindowInterface2.footnoteVisible

                    onCurrentIndexChanged: {
                        positionViewAtIndex(currentIndex,ListView.Beginning)
                    }

                    delegate: Text {
                        text: line
                        textFormat: Text.RichText
                        width: parent.width
                        color: btStyle.textColor
                        font.family: btWindowInterface2.fontName
                        font.pointSize: btWindowInterface2.fontSize
                        wrapMode: Text.WordWrap
                        onWidthChanged: doLayout()
                    }
                }
            }
        }

        BtStyle {
            id: btStyle
        }
    }
}
