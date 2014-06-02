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

Column {
    id: menus

    width:120
    height:20 * listModel.count
    anchors.top: mainToolbar.bottom
    anchors.right: mainToolbar.right
    z: 100
    visible: false

    Repeater {
        model: listModel
        delegate: Rectangle {
            width:parent.width
            height:24
            color: btStyle.menu
            border.color: btStyle.menuBorder
            border.width: 1
            Text {
                anchors.centerIn: parent
                text: title
                color: btStyle.menuText
            }
        }
    }
}
