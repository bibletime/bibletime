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
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import BibleTime 1.0

SplitView {
    id: searchResults

//    property string searchText: ""
//    property string findChoice: ""
//    property string moduleList: ""

    property alias searchText: btSearchInterface.searchText
    property alias findChoice: btSearchInterface.findChoice
    property alias moduleList: btSearchInterface.moduleList

    property real leftRightSplit: 0.35
    property real topBottomSplit: 0.35
    property real handleWidth: btStyle.pixelsPerMillimeterX *1.5
    property alias modulesModel: modulesView.model

    orientation: Qt.Vertical
    handleDelegate: Rectangle { width: 1; height: handleWidth; color: "gray" }
    anchors.fill: parent
    visible: false
    onWidthChanged: {
        if ( ! visible)
            return;
    }

    onVisibleChanged: {
        if (visible) {
            console.log("search:", searchText, findChoice, moduleList);
            btWindowInterface.moduleName = "ESV";
            btWindowInterface.reference = "Genesis 1:1";
            btSearchInterface.performSearch();
        }
    }

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && searchResults.visible == true) {
            searchResults.visible = false;
            event.accepted = true;
        }
    }

    Rectangle {
        id: topArea

        width: parent.width
        height: parent.height * searchResults.topBottomSplit

        SplitView {
            id: topSplitter
            orientation: Qt.Horizontal
            anchors.fill: parent
            handleDelegate: Rectangle { width: handleWidth; height: 2; color: "gray" }

            Rectangle {
                id: modules

                color: "white"
                anchors.left: parent.left
                anchors.top: parent.top
                width: parent.width * searchResults.leftRightSplit
                height: parent.height
                border.color: "black"
                border.width: 1

                TitleColorBar {
                    id: titleBar1
                    title: "Works"
                    width: parent.width
                    anchors.left: parent.left
                    anchors.top: parent.top
                }

                ListSelectView {
                    id: modulesView

                    anchors.top: titleBar1.bottom
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    leftTextMargin: 30
                }
            }

            Rectangle {
                id: references

                color: "white"
                anchors.left: modules.right
                anchors.top: parent.top
                Layout.fillWidth: true
                border.color: "black"
                border.width: 1

                TitleColorBar {
                    id: titleBar2
                    title: "References"
                    width: parent.width
                }

                ListView {
                    id: referencesView

                    anchors.top: titleBar2.bottom
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 3
                    anchors.rightMargin: 3
                    anchors.bottomMargin: 3
                    clip: true
                    highlightFollowsCurrentItem: true
                    currentIndex: 2
                }
            }
        }
    }

    Rectangle {
        id: text

        color: "white"
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: parent.width
        Layout.fillWidth: true
        border.color: "black"
        border.width: 1

        TitleColorBar {
            id: titleBar3
            title: "Text"
            width: parent.width
        }

        ListView {
            id: listView

            clip: true
            anchors.top: titleBar3.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            model: btWindowInterface.textModel
            highlightFollowsCurrentItem: true
            currentIndex: btWindowInterface.currentModelIndex
            onCurrentIndexChanged: {
                positionViewAtIndex(currentIndex,listView.Beginning)
            }

            delegate: Text {
                text: line
                textFormat: Text.RichText
                width: parent.width
                color: "black"
                font.pointSize: btStyle.textFontPointSize
                wrapMode: Text.WordWrap
                onWidthChanged: doLayout()
            }
        }
    }

    BtWindowInterface {
        id: btWindowInterface
    }

    BtSearchInterface {
        id: btSearchInterface
    }
}
