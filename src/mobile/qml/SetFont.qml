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
import QtQuick.Layouts 1.1
import BibleTime 1.0

Rectangle {
    id: setFont

    property string language: ""
    property bool ready: false

    signal textFontChanged

    height: languageRow.height + slider.height + buttons.height + buttons.anchors.bottomMargin + languageRow.anchors.bottomMargin +btStyle.pixelsPerMillimeterX*7
    width: {
        var width = Math.min(parent.width, parent.height);
        width = width - 2 * anchors.rightMargin
        return width;
    }
    color: btStyle.textBackgroundColor
    border.width: 3
    border.color: btStyle.textColor
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.bottomMargin: btStyle.pixelsPerMillimeterX * 2
    anchors.rightMargin: btStyle.pixelsPerMillimeterX * 2

    onVisibleChanged: {
        setFont.ready = false;
        if (visible) {
            updateLanguageCombo(setFont.language)
            var index = languageCombo.currentIndex;
            updateUiFontNameAndSize(index);
            moduleInterface.saveCurrentFonts();
            setFont.ready = true;
        }
    }

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && setFont.visible == true) {
            moduleInterface.restoreSavedFonts();
            setFont.textFontChanged();
            setFont.visible = false;
            event.accepted = true;
        }
    }

    function updateUiFontNameAndSize(index) {
        var language = languageCombo.textAt(index);
        var fontSize = moduleInterface.getFontSizeForLanguage(language);
        slider.value = fontSize;
        var fontName = moduleInterface.getFontNameForLanguage(language);
        updateFontNameCombo(fontName);
    }

    function updateLanguageCombo(language) {
        languageCombo.model = moduleInterface.installedModuleLanguages();
        var index = languageCombo.find(language);
        if (index >= 0)
            languageCombo.currentIndex = index;
    }

    function updateFontNameCombo(fontName) {
        var index = fontCombo.find(fontName);
        if (index >= 0)
            fontCombo.currentIndex = index;
    }

    function setFontForLanguage(fontName) {
        if (! setFont.ready)
            return;
        var language = languageCombo.currentText;
        var fontSize = slider.value;
        moduleInterface.setFontForLanguage(language, fontName, fontSize);
        setFont.textFontChanged();
    }

    ModuleInterface {
        id: moduleInterface
    }

    Grid {
        id: languageRow

        rows: 3
        columns: 2

        anchors.top: parent.top
        anchors.topMargin: btStyle.pixelsPerMillimeterX * 4
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: btStyle.pixelsPerMillimeterX * 4

        Text {
            id: title
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            text: qsTr("For Language")
            font.pointSize: btStyle.uiFontPointSize
            color: btStyle.textColor
        }

        ComboBox {
            id: languageCombo

            width: {
                var width = setFont.width;
                width = width - Math.max(title.width, fontText.width, fontSize.width);
                width = width - languageRow.spacing * 3;
                return width;
            }
            Layout.fillWidth: true
            currentIndex: 0
            style: ComboBoxStyle {
                label: Text {
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: btStyle.uiFontPointSize
                    color: "black"
                    text: control.currentText
                    elide: Text.ElideRight
                }
            }

            onActivated: {
                updateUiFontNameAndSize(index);
            }
        }

        Text {
            id: fontText
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            text: qsTr("Font")
            font.pointSize: btStyle.uiFontPointSize
            color: btStyle.textColor
        }

        ComboBox {
            id: fontCombo

            model: Qt.fontFamilies()
            width: languageCombo.width
            height: languageCombo.height
            Layout.fillWidth: true
            style: ComboBoxStyle {
                label: Text {
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: btStyle.uiFontPointSize
                    color: "black"
                    text: control.currentText
                    elide: Text.ElideRight
                }
            }
            onActivated: {
                var fontName = fontCombo.textAt(index);
                setFont.setFontForLanguage(fontName);
            }

            BtStyle {
                id: btStyle
            }
        }

        Text {
            id: fontSize
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            text: qsTr("Font Size")
            font.pointSize: btStyle.uiFontPointSize
            color: btStyle.textColor
        }

        Slider {
            id: slider

            width: languageCombo.width
            height: fontSize.height * 1.2
            minimumValue: 10
            maximumValue: 30
            style: SliderStyle {
                    groove: Rectangle {
                        implicitWidth: 200
                        implicitHeight: 8
                        color: btStyle.textColor
                        radius: 8
                    }
                    handle: Rectangle {
                        anchors.centerIn: parent
                        color: btStyle.textBackgroundColor
                        border.color: btStyle.textColor
                        border.width: 3
                        implicitWidth: btStyle.pixelsPerMillimeterY * 7
                        implicitHeight: btStyle.pixelsPerMillimeterY * 7
                        radius: btStyle.pixelsPerMillimeterY * 3.5
                    }
                }
            onValueChanged: {

                var fontName = fontCombo.currentText
                setFont.setFontForLanguage(fontName);
            }

        }
    }


    Grid {
        id: buttons

        spacing: btStyle.pixelsPerMillimeterY * 5
        columns: 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: btStyle.pixelsPerMillimeterX * 4

        Action {
            id: okAction
            text: qsTr("Ok")
            onTriggered: {
                setFont.visible = false;
            }
        }

        Button {
            id: okButton
            height: fontCombo.height * 1.2
            width: setFont.width/3.5
            action: okAction
            style: BtButtonStyle {
            }
        }

        Action {
            id: cancelAction
            text: qsTr("Cancel")
            onTriggered: {
                setFont.visible = false;
                moduleInterface.restoreSavedFonts();
                setFont.textFontChanged();
            }
        }

        Button {
            id: cancelButton
            height: fontCombo.height * 1.2
            width: setFont.width/3.5
            action: cancelAction
            style: BtButtonStyle {
            }
        }
    }
}
