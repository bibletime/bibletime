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

    signal finished()

    function start(moduleName, reference) {
        atState = "BOOK"
        module = moduleName;
        gridChooser.accepted.disconnect(verseChooser.accepted);
        gridChooser.accepted.connect(verseChooser.accepted);
        gridChooser.model = chooserInterface.getBooks(module);
        gridChooser.selected = "";
        gridChooser.titleText = qsTr("Book");
        gridChooser.visible = true;
    }

    function accepted(value) {
        console.log("accepted", atState)
        if (atState == "BOOK") {
            book = value;
            atState = "CHAPTER";
            gridChooser.model  = chooserInterface.getChapters(module, book);
            gridChooser.selected = "";
            gridChooser.titleText = qsTr("Chapter");
            gridChooser.visible = true;
        }
        else if (atState == "CHAPTER") {
            chapter = value;
            atState = "VERSE";
            gridChooser.model  = chooserInterface.getVerses(module, book, chapter);
            gridChooser.selected = "";
            gridChooser.titleText = qsTr("Verse");
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

    ChooserInterface {
        id: chooserInterface
    }
}
