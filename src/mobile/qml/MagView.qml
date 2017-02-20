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

    signal magFinished();

    function setModule(module) {
        btWindowInterface.moduleName = module;
    }

    BtWindowInterface {
        id: btWindowInterface
        onReferenceChange: {
            console.log("reference changed: ",btWindowInterface.moduleName, " ", btWindowInterface.reference);
        }
        onModuleChanged: {
            console.log("module changed");
        }
    }

    Rectangle {

        anchors.fill: parent
        color: "lightblue"
        //color: btStyle.textBackgroundColor

        Rectangle {
            id: toolbar

            height: {
                var pixel = btStyle.pixelsPerMillimeterY * 8;
                var uiFont = btStyle.uiFontPointSize * 4.4;
                var mix = pixel * 0.7 + uiFont * 0.3;
                return Math.max(pixel, mix);
            }
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 1
            color: btStyle.toolbarColor

            Rectangle {
                id: moduleDisplay

                width: text.width + 30
                radius:btStyle.pixelsPerMillimeterX
                anchors.left: parent.left
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
                    var w2 = toolbar.width - moduleDisplay.width - toolbar.height * 2
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

        BtStyle {
            id: btStyle
        }
    }

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && magView.focus == true) {
            console.log("key")
            magFinished();
            event.accepted = true;
        }
    }
}
