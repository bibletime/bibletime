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
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import BibleTime 1.0

Rectangle {
    id: defaultDoc

    color: btStyle.textBackgroundColor
    anchors.fill: parent

    BtStyle {
        id: btStyle
    }

    BtmConfig {
        id: btmConfig
    }

    Rectangle {
        id: standardDocTitleBar
        color: btStyle.toolbarColor
        width: parent.width
        height: btStyle.pixelsPerMillimeterY * 7

        Back {
            id: backTool

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            text: qsTranslate("Navigation", "Main")
            onClicked: {
                defaultDoc.visible = false;
            }
        }
    }

    Flickable {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: standardDocTitleBar.bottom
        anchors.bottom: parent.bottom
        anchors.topMargin: btStyle.pixelsPerMillimeterY * 2
        contentWidth: width
        contentHeight: column.height * 1.1

        onVisibleChanged: {
            if (visible) {
                btmConfig.initializeAvailableModules();
            }
        }

        Column {
            id: column
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: btStyle.pixelsPerMillimeterX * 10
            width: btStyle.pixelsPerMillimeterX * 50
            spacing: btStyle.pixelsPerMillimeterY * 2

            Rectangle {
                id: spacer0

                color: btStyle.textBackgroundColor
                width:1
                height: btStyle.pixelsPerMillimeterY * 2
            }

            Text {
                id: titleText

                text: qsTranslate("Default Documents", "Standard Documents")
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: btStyle.uiFontPointSize + 4
                color: btStyle.textColor
            }

            Rectangle {
                id: spacer1

                color: btStyle.textBackgroundColor
                width:1
                height: btStyle.pixelsPerMillimeterY * 4
            }

            Text {
                id: bibleText

                text: qsTranslate("Default Documents", "Bible:")
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: btStyle.uiFontPointSize
                color: btStyle.textColor
            }

            ComboBox {
                id: bibleCombo

                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                currentIndex: {
                    var bibles = btmConfig.availableBibles;
                    var defaultBible = btmConfig.getDefaultSwordModuleByType("standardBible");
                    var index = bibles.indexOf(defaultBible);
                    return index;
                }
                onActivated: {
                    var bibles = btmConfig.availableBibles;
                    var bible = bibles[index];
                    btmConfig.setDefaultSwordModuleByType("standardBible", bible);
                }

                model: btmConfig.availableBibles
                style: ComboBoxStyle {
                    label: Text {
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: btStyle.uiFontPointSize
                        color: "black"
                        text: control.currentText
                        elide: Text.ElideRight
                    }
                }
            }

            Rectangle {
                id: spacer2

                color: btStyle.textBackgroundColor
                width:1
                height: btStyle.pixelsPerMillimeterY * 4
            }

            Text {
                id: hebrewStrongsText

                text: qsTranslate("Default Documents", "Hebrew Strong's lexicon:")
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: btStyle.uiFontPointSize
                color: btStyle.textColor
            }

            ComboBox {
                id: hebrewStrongsCombo

                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                model: btmConfig.availableHebrewStrongsLexicons
                currentIndex: {
                    var modules = btmConfig.availableHebrewStrongsLexicons;
                    var defaultModule = btmConfig.getDefaultSwordModuleByType("standardHebrewStrongsLexicon");
                    var index = modules.indexOf(defaultModule);
                    return index;
                }
                onActivated: {
                    var modules = btmConfig.availableHebrewStrongsLexicons
                    var defaultModule = modules[index];
                    btmConfig.setDefaultSwordModuleByType("standardHebrewStrongsLexicon", defaultModule);
                }
                style: ComboBoxStyle {
                    label: Text {
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: btStyle.uiFontPointSize
                        color: "black"
                        text: control.currentText
                        elide: Text.ElideRight
                    }
                }
            }

            Rectangle {
                id: spacer3

                color: btStyle.textBackgroundColor
                width:1
                height: btStyle.pixelsPerMillimeterY * 4
            }

            Text {
                id: greekStrongsText

                text: qsTranslate("Default Documents", "Greek Strong's lexicon:")
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: btStyle.uiFontPointSize
                color: btStyle.textColor
            }

            ComboBox {
                id: greekStrongsCombo

                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                currentIndex: {
                    var modules = btmConfig.availableGreekStrongsLexicons;
                    var defaultModule = btmConfig.getDefaultSwordModuleByType("standardGreekStrongsLexicon");
                    var index = modules.indexOf(defaultModule);
                    return index;
                }
                onActivated: {
                    var modules = btmConfig.availableGreekStrongsLexicons
                    var defaultModule = modules[index];
                    btmConfig.setDefaultSwordModuleByType("standardGreekStrongsLexicon", defaultModule);
                }
                model: btmConfig.availableGreekStrongsLexicons
                style: ComboBoxStyle {
                    label: Text {
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: btStyle.uiFontPointSize
                        color: "black"
                        text: control.currentText
                        elide: Text.ElideRight
                    }
                }
            }
            Rectangle {
                id: spacer4

                color: btStyle.textBackgroundColor
                width:1
                height: btStyle.pixelsPerMillimeterY * 4
            }

            Text {
                id: hebrewMorphText

                text: qsTranslate("Default Documents", "Hebrew Morphological lexicon:")
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: btStyle.uiFontPointSize
                color: btStyle.textColor
            }

            ComboBox {
                id: hebrewMorphCombo

                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                model: btmConfig.availableHebrewMorphLexicons
                currentIndex: {
                    var modules = btmConfig.availableHebrewMorphLexicons;
                    var defaultModule = btmConfig.getDefaultSwordModuleByType("standardHebrewMorphLexicon");
                    var index = modules.indexOf(defaultModule);
                    return index;
                }
                onActivated: {
                    var modules = btmConfig.availableHebrewMorphLexicons
                    var defaultModule = modules[index];
                    btmConfig.setDefaultSwordModuleByType("standardHebrewMorphLexicon", defaultModule);
                }
                style: ComboBoxStyle {
                    label: Text {
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: btStyle.uiFontPointSize
                        color: "black"
                        text: control.currentText
                        elide: Text.ElideRight
                    }
                }
            }

            Rectangle {
                id: spacer5

                color: btStyle.textBackgroundColor
                width:1
                height: btStyle.pixelsPerMillimeterY * 4
            }

            Text {
                id: greekMorphText

                text: qsTranslate("Default Documents", "Greek Morphological lexicon:")
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: btStyle.uiFontPointSize
                color: btStyle.textColor
            }

            ComboBox {
                id: greekMorphCombo

                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                currentIndex: {
                    var modules = btmConfig.availableGreekMorphLexicons;
                    var defaultModule = btmConfig.getDefaultSwordModuleByType("standardGreekMorphLexicon");
                    var index = modules.indexOf(defaultModule);
                    return index;
                }
                onActivated: {
                    var modules = btmConfig.availableGreekMorphLexicons
                    var defaultModule = modules[index];
                    btmConfig.setDefaultSwordModuleByType("standardGreekMorphLexicon", defaultModule);
                }
                model: btmConfig.availableGreekMorphLexicons
                style: ComboBoxStyle {
                    label: Text {
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: btStyle.uiFontPointSize
                        color: "black"
                        text: control.currentText
                        elide: Text.ElideRight
                    }
                }
            }
        }
    }

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && defaultDoc.visible == true) {
            defaultDoc.visible = false;
            event.accepted = true;
        }
    }
}
