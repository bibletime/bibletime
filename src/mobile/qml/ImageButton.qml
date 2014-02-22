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

import QtQuick 2.1

Rectangle {
    id: imageButton

    property bool show: true
    property string icon: ""
    property int corner: 6

    border.width: 1
    border.color: "gray"
    radius: corner

    Image{
        id: nextIcon
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        source: imageButton.icon
        height: parent.height
        width: parent.height
        anchors.right: parent.right
        anchors.top: parent.top
    }
}
