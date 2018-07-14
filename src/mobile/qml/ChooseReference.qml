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
    id: chooseReference

    property string moduleName: ""
    property string reference: ""

    signal finished()
    signal finishedLexicon(int index)

    function start(moduleName, reference, backText) {
        chooseReference.moduleName = moduleName;
        chooseReference.reference = reference;
        if (chooserInterface.isBibleOrCommentary(moduleName)) {
            verseChooser.finished.disconnect(chooseReference.verseReferenceChoosen)
            verseChooser.finished.connect(chooseReference.verseReferenceChoosen)
            verseChooser.backText = backText;
            verseChooser.start(moduleName, reference);
        }
        else if (chooserInterface.isBook(moduleName)) {
            chooserInterface.setupTree(moduleName, reference);
            treeChooser.model = chooserInterface.getBookModel();
            treeChooser.path = chooserInterface.getBackPath(reference);
            treeChooser.visible = true;
        }
    }

    function next(value) {
        chooserInterface.next(value);
        treeChooser.model = chooserInterface.getBookModel();
        treeChooser.path = chooserInterface.getBackPath(reference);
    }

    function back() {
        chooserInterface.back();
        treeChooser.model = chooserInterface.getBookModel();
        treeChooser.path = chooserInterface.getBackPath(reference);
    }

    function select(name) {
        chooserInterface.select(name);
        reference = chooserInterface.getTreeReference();
        treeChooser.visible = false;
        finished();
    }

    function verseReferenceChoosen() {
        reference = verseChooser.reference;
        finished();
    }

    function startLexicon(moduleName, reference, model, index) {
        keyNameChooser.model = model;
        keyNameChooser.itemSelected.disconnect(chooseReference.lexiconReferenceChoosen)
        keyNameChooser.itemSelected.connect(chooseReference.lexiconReferenceChoosen)
        keyNameChooser.currentIndex = index;
        keyNameChooser.visible = true;
    }

    function lexiconReferenceChoosen(index) {
        finishedLexicon(index);
    }

    ChooserInterface {
        id: chooserInterface
    }
}
