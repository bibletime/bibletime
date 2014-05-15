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
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import BibleTime 1.0

Rectangle {
    id: aboutDialog

    color: "white"
    anchors.fill: parent

    BtStyle {
        id: btStyle
    }

    Flickable {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        contentWidth: width
        contentHeight: column.height * 1.1

        Column {
            id: column
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            spacing: btStyle.pixelsPerMillimeterY * 2

            Image {
                id: logo

                width:  aboutDialog.width/8
                height: width
                anchors.horizontalCenter: parent.horizontalCenter
                source: "qrc:/share/bibletime/icons/bibletime.svg"
            }

            Text {
                id: titleText

                text: "BibleTime Mobile"
                font.pointSize: btStyle.uiFontPointSize + 2
            }

            Text {
                id: urlText

                wrapMode: Text.Wrap
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                text: "Brought to you by the <a href=\"http://www.bibletime.info\">BibleTime Team</a>."
                font.pointSize: btStyle.uiFontPointSize
                onLinkActivated: {
                    console.log(link)
                    Qt.openUrlExternally(link);
                }
            }

            Text {
                id: versionText

                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
                text: formatVersion();
                font.pointSize: btStyle.uiFontPointSize

                function formatVersion(version) {
                    var appstr = "Version\n"
                    appstr += btStyle.appVersion;
                    appstr += "\n\n";

                    var gitstr = "Git version\n";
                    var gitversion = btStyle.gitVersion;
                    var len = gitversion.length;
                    gitstr += gitversion.substring(0, len/2-1);
                    gitstr += "\n";
                    gitstr += gitversion.substring(len/2, len);
                    gitstr += "\n\n";

                    var qtstr = "Qt version\n"
                    qtstr += btStyle.qtVersion;
                    qtstr += "\n\n";

                    var swordstr = "Sword version\n"
                    swordstr += btStyle.swordVersion;
                    swordstr += "\n\n";

                    return appstr + gitstr + qtstr + swordstr;
                }
            }
        }
    }

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && aboutDialog.visible == true) {
            aboutDialog.visible = false;
            event.accepted = true;
        }
    }
}
