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

    function setupSearch() {
        InputMethod.hide = true; // hide keyboard
        searchText = textInput.text;
        if (radioAny.checked)
            findChoice = "or";
        if (radioAll.checked)
            findChoice = "and";
        if (radioPhrase.checked)
            findChoice = "regexpr";
        moduleList = searchComboBox.currentText;
        searchRequest();
    }
    anchors.fill: parent
    visible: false
    onVisibleChanged: {
        if (visible) {
            textInput.text = searchText;
        }
    }

    Rectangle{

        anchors.fill: parent
        color: "lightgray"

        Rectangle {
            id: searchTitleBar
            color: btStyle.toolbarColor
            width: parent.width
            height: btStyle.pixelsPerMillimeterY * 7
            border.color: "black"
            border.width: 1

            Text {
                id: title
                color: "black"
                font.pointSize: btStyle.uiFontPointSize
                anchors.left: parent.left
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

                Rectangle {
                    color: "white"
                    width: search.width -searchButton.width - search.spacing * 3
                    height:searchButton.height

                    TextField {
                        id: textInput

                        anchors.fill: parent
                        font.pointSize: btStyle.uiFontPointSize
                        verticalAlignment: Text.AlignVCenter
                        inputMethodHints: Qt.ImhNoAutoUppercase
                        focus: true
                        text: ""
                        onAccepted: {
                            search.setupSearch();
                        }
                    }
                }
            }

            Button {
                id: searchButton
                text: "Search"
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
        }


        GroupBox {
            id: findWords

            anchors.top: titleText.bottom
            anchors.left: parent.left
            anchors.leftMargin: search.spacing
            title: ""

            Row {
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
                text: "Search in"
                font.pointSize: btStyle.uiFontPointSize
                height: searchInLabel.contentHeight
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

                model: [ "ESV, KJV, Josephus", "ESV", "KJV", "Josephus" ]
            }
        }



        BtStyle {
            id: btStyle
        }
    }

    Keys.onReleased: {
        if ((event.key == Qt.Key_Back || event.key == Qt.Key_Escape) && search.visible == true) {
            search.visible = false;
            event.accepted = true;
        }
    }
}
