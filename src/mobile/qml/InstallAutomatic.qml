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
        installInterface.clearModules();
        installInterface.addModule("CrossWire","ESV2011");
        installInterface.addModule("CrossWire","KJV");
        installInterface.addModule("CrossWire","StrongsGreek");
        installInterface.addModule("CrossWire","StrongsHebrew");
        installInterface.installModulesAuto();
        installInterface.modulesDownloadFinished.connect(openFirstWindow);
    }

    function openFirstWindow() {
        installInterface.modulesDownloadFinished.disconnect(openFirstWindow);
        installAutomatic.visible = false;
        installAutomatic.finished();
    }

    Image {
        id: logo

        width: btStyle.pixelsPerMillimeterX*12
        height: width
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: width * 0.2
        anchors.leftMargin: width * 0.2
        source: "qrc:/share/bibletime/icons/bibletime.svg"
    }

    Text {
        id: installAutoTitle

        color: btStyle.textColor
        font.pointSize: btStyle.uiFontPointSize
        text: "BibleTime"
        anchors.left: logo.right
        anchors.verticalCenter: logo.verticalCenter
        anchors.leftMargin: logo.width * 0.2
    }
}
