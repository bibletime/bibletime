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
import QtQuick.Controls.Private 1.0
import BibleTime 1.0

FocusScope {
    id: search

    property int spacing: btStyle.pixelsPerMillimeterX * 1.5
    property string searchText: ""
    property string findChoice: ""
    property string moduleList: ""
    property alias moduleChoices: searchComboBox.model

    signal searchRequest();
    signal searchFinished();

    function setupSearch() {
        Qt.inputMethod.hide(); // hide keyboard
        searchText = textInput.displayText;
        if (radioAny.checked)
            findChoice = "or";
        if (radioAll.checked)
            findChoice = "and";
        if (radioPhrase.checked)
            findChoice = "regexpr";
        moduleList = searchComboBox.currentText;
        searchRequest();
    }

    onFocusChanged: {
        if (focus) {
            textInput.text = searchText;
        }
    }

    Rectangle {

        anchors.fill: parent
        color: btStyle.textBackgroundColor

        Rectangle {
            id: searchTitleBar
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
                    search.searchFinished();
                }
            }

            Text {
                id: title
                color: btStyle.toolbarTextColor
                font.pointSize: btStyle.uiFontPointSize
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.leftMargin: search.spacing
                verticalAlignment: Text.AlignVCenter
                text: qsTranslate("Search", "Search")
            }
        }

        Row {
            id: searchInput

            anchors.left: parent.left
            anchors.top: searchTitleBar.bottom
            anchors.leftMargin: search.spacing
            anchors.topMargin: btStyle.pixelsPerMillimeterY*5
            spacing: search.spacing

            Column {

                spacing: btStyle.pixelsPerMillimeterY

                    TextField {
                        id: textInput

                        width: search.width -searchButton.width - search.spacing * 3
                        height: searchIn.height*1.4
                        font.pointSize: btStyle.uiFontPointSize
                        verticalAlignment: Text.AlignVCenter
                        inputMethodHints: Qt.ImhNoAutoUppercase
                        focus: true
                        text: ""
                        textColor: btStyle.textColor

                        style: TextFieldStyle {
                            textColor: btStyle.textColor
                            background: Rectangle {
                                radius: 6
                                anchors.fill: parent
                                border.color: btStyle.textColor
                                border.width: 2
                                color: btStyle.textBackgroundColor
                            }
                        }

                        onAccepted: {
                            search.setupSearch();
                        }
                }
            }

            Button {
                id: searchButton
                text: qsTranslate("Search", "Search")
                height: textInput.height
                style: ButtonStyle {
                    label: Text {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.right: background.right
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: btStyle.uiFontPointSize
                        color: "black"
                        text: control.text
                    }
                }
                onClicked: {
                    search.setupSearch();
                }
            }
        }

        Text {
            id: titleText

            anchors.left: parent.left
            anchors.top: searchInput.bottom
            anchors.leftMargin: search.spacing
            anchors.topMargin: btStyle.pixelsPerMillimeterY*5
            font.pointSize: btStyle.uiFontPointSize
            text: qsTr("Find")
            color: btStyle.textColor
        }

        GroupBox {
            id: findWords

            anchors.top: titleText.bottom
            anchors.left: parent.left
            anchors.leftMargin: search.spacing
            title: ""

            Column {
                spacing: 30
                ExclusiveGroup { id: group }
                RadioButton {
                    id: radioAll

                    text: qsTr("All Words")
                    exclusiveGroup: group
                    style: RadioButtonStyle {

                        label: Label {
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            font.pointSize: btStyle.uiFontPointSize
                            text: control.text
                            color: btStyle.textColor
                        }
                    }
                    checked: true
                }
                RadioButton {
                    id: radioAny

                    text: qsTr("Any Word")
                    exclusiveGroup: group
                    style: RadioButtonStyle {

                        label: Label {
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            font.pointSize: btStyle.uiFontPointSize
                            text: control.text
                            color: btStyle.textColor
                        }
                    }
                }
                RadioButton {
                    id: radioPhrase

                    text: qsTr("Regular Expression")
                    exclusiveGroup: group
                    style: RadioButtonStyle {

                        label: Label {
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            font.pointSize: btStyle.uiFontPointSize
                            text: control.text
                            color: btStyle.textColor
                        }
                    }
                }
            }
        }

        Row {
            id: searchIn

            anchors.left: parent.left
            anchors.top: findWords.bottom
            anchors.topMargin: btStyle.pixelsPerMillimeterY*5
            anchors.leftMargin: search.spacing
            spacing: search.spacing

            Text {
                id: searchInLabel
                text: qsTranslate("Search", "Search in")
                font.pointSize: btStyle.uiFontPointSize
                color: btStyle.textColor
            }

            ComboBox {
                id: searchComboBox

                width: search.width * 2 / 3


                style: ComboBoxStyle {
                    label: Text {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: background.right
                        anchors.leftMargin: search.spacing
                        font.pointSize: btStyle.uiFontPointSize
                        color: "black"
                        text: control.currentText
                    }
                }

            }
        }



        BtStyle {
            id: btStyle
        }
    }

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && search.focus == true) {
            searchFinished();
            event.accepted = true;
        }
    }
}
