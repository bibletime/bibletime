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

ButtonStyle {
    id: btButtonStyle

    property bool bold: false
    property bool italic: false
    property bool underline: false

    background: Rectangle {
        border.width: 2
        border.color: "gray"
        radius: btStyle.pixelsPerMillimeterX
        color: btStyle.textBackgroundColor
        gradient: Gradient {
            GradientStop { position: 0 ; color: !control.checked ? btStyle.buttonGradient0 : btStyle.buttonGradient1 }
            GradientStop { position: 1 ; color: !control.checked ? btStyle.buttonGradient2 : btStyle.buttonGradient3 }
        }
    }

    label: Text {
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: btStyle.uiFontPointSize
        color: !control.checked ? btStyle.textColor : btStyle.textBackgroundColor
        text: control.text
        font.bold: btButtonStyle.bold
        font.italic: btButtonStyle.italic
        font.underline: btButtonStyle.underline
    }
}
