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
import QtQuick.Controls 2.2
import QtQuick.Controls 1.2
import BibleTime 1.0

Rectangle {
    id: installAutomatic

    anchors.fill: parent
    color: btStyle.textBackgroundColor
    onVisibleChanged: {
        if (visible)
            install();
    }

    signal finished

    function install() {
        installInterface.progressFinished.connect(autoModuleInstall);
        installInterface.refreshLists("","","");
    }

    function autoModuleInstall() {
        installInterface.progressFinished.disconnect(autoModuleInstall);
        if (installInterface.wasCanceled) {
            installAutomatic.visible = false;
            return;
        }
        installInterface.clearModules();
        installInterface.addModule("CrossWire","ESV2011");
        installInterface.addModule("CrossWire","KJV");
        installInterface.addModule("CrossWire","StrongsGreek");
        installInterface.addModule("CrossWire","StrongsHebrew");
        installInterface.installModulesAuto();
        installInterface.modulesDownloadFinished.disconnect(openFirstWindow);
        installInterface.modulesDownloadFinished.connect(openFirstWindow);
        installInterface.wasCanceledChanged.disconnect(cancelWindowOpening);
        installInterface.wasCanceledChanged.connect(cancelWindowOpening);
    }

    function cancelWindowOpening() {
        installInterface.modulesDownloadFinished.disconnect(openFirstWindow);
        installInterface.wasCanceledChanged.disconnect(cancelWindowOpening);
        installAutomatic.visible = false;
    }

    function openFirstWindow() {
        installInterface.modulesDownloadFinished.disconnect(openFirstWindow);
        installInterface.wasCanceledChanged.disconnect(cancelWindowOpening);
        installAutomatic.visible = false;
        if (installAutomatic.wasCanceled) {
            return;
        }
        installAutomatic.finished();
    }

    Rectangle {
        id: titleBar
        color: btStyle.toolbarColor
        width: parent.width
        anchors.margins: btStyle.pixelsPerMillimeterX * 2
        height: {
            var pixel = btStyle.pixelsPerMillimeterY * 7.5;
            var uiFont = btStyle.uiFontPointSize * 4.4;
            var mix = pixel * 0.7 + uiFont * 0.3;
            return Math.max(pixel, mix);
        }

        Image {
            id: logo

            width: parent.height - 10
            height: parent.height - 10
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: width * 0.1
            anchors.leftMargin: width * 0.1
            source: "qrc:/share/bibletime/icons/bibletime.svg"
        }

        Text {
            id: title

            color: btStyle.toolbarTextColor
            font.pointSize: btStyle.uiFontPointSize * 1.1
            text: qsTr("BibleTime Mobile")
            anchors.left: logo.right
            anchors.verticalCenter: logo.verticalCenter
            anchors.leftMargin: logo.width * 0.2
        }
    }
}
