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
import BibleTime 1.0

Item {
    id: verseChooser

    property string module: ""
    property string book: ""
    property string chapter: ""
    property string verse: ""
    property string reference: ""
    property string atState: ""
    property string backText: ""

    signal finished()

    function start(moduleName, reference) {
        module = moduleName;
        gridChooser.backText = backText;
        gridChooser.accepted.disconnect(verseChooser.accepted);
        gridChooser.accepted.connect(verseChooser.accepted);
        gridChooser.backup.disconnect(verseChooser.goBack);
        gridChooser.backup.connect(verseChooser.goBack);
        setupBook();
        gridChooser.visible = true;
    }

    function accepted(value) {
        if (atState == "BOOK") {
            book = value;
            setupChapter();
            gridChooser.visible = true;
        }
        else if (atState == "CHAPTER") {
            chapter = value;
            setupVerse();
            gridChooser.visible = true;
        }
        else if (atState == "VERSE") {
            gridChooser.accepted.disconnect(verseChooser.accepted);
            verse = value;
            atState = "";
            reference = book + " " + chapter + ":" + verse;
            finished();
        }
    }

    function goBack() {
        if (atState == "BOOK") {
            gridChooser.accepted.disconnect(verseChooser.accepted);
            gridChooser.visible = false;
        }
        else if (atState == "CHAPTER") {
            setupBook();
            gridChooser.visible = true;
        }
        else if (atState == "VERSE") {
            setupChapter();
            gridChooser.visible = true;
        }
    }

    function setupBook() {
        atState = "BOOK";
        gridChooser.model = chooserInterface.getBooks(module);
        gridChooser.selected = "";
        gridChooser.titleText = qsTr("Book");
        gridChooser.backText = verseChooser.backText;
    }

    function setupChapter() {
        atState = "CHAPTER";
        gridChooser.model  = chooserInterface.getChapters(module, book);
        gridChooser.selected = "";
        gridChooser.titleText = qsTr("Chapter");
        gridChooser.backText = qsTr("Book");
    }

    function setupVerse() {
        atState = "VERSE";
        gridChooser.model  = chooserInterface.getVerses(module, book, chapter);
        gridChooser.selected = "";
        gridChooser.titleText = qsTr("Verse");
        gridChooser.backText = qsTr("Chapter");
    }

    ChooserInterface {
        id: chooserInterface
    }
}
