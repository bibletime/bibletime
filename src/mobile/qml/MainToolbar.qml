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
    id: toolbar

    color: btStyle.toolbarColor
    z:0
    onEnabledChanged: {
        menuButton.visible = toolbar.enabled
        searchIcon.visible = toolbar.enabled
    }

    signal buttonClicked
    signal searchClicked

    BtStyle {
        id: btStyle
    }

    Image {
        id: logo

        width: parent.height-10
        height: parent.height-10
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.bottomMargin: 5
        anchors.leftMargin: 10
        source: "qrc:/share/bibletime/icons/bibletime.svg"
    }

    Text {
        id: title
        color: btStyle.toolbarTextColor
        font.pointSize: btStyle.uiFontPointSize
        anchors.left: logo.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: 20
        verticalAlignment: Text.AlignVCenter
        text: qsTranslate("MainToolbar", "BibleTime Mobile")
    }

    MenuButton {
        id: menuButton

        width: parent.height
        height: parent.height
        anchors.right: parent.right
        anchors.top: parent.top
        foreground: btStyle.toolbarTextColor
        background: btStyle.toolbarColor
        onButtonClicked: {
            toolbar.buttonClicked()
        }
    }

    SearchIcon {
        id: searchIconQml

        width: parent.height
        height: parent.height
        anchors.right: menuButton.left
        anchors.top: parent.top
        anchors.rightMargin: 0
        strokeStyle: btStyle.toolbarTextColor

        MouseArea {
            anchors.fill: parent
            onClicked: {
                toolbar.searchClicked()
            }
        }
    }
}
