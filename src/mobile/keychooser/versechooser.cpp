/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "versechooser.h"

#include <cmath>
#include "mobile/ui/btwindowinterface.h"
#include "mobile/ui/gridchooser.h"
#include "mobile/ui/qtquick2applicationviewer.h"
#include <QQuickItem>
#include <QQmlProperty>
#include <QQmlContext>
#include <QDebug>
#include <QQmlProperty>
#include <QCoreApplication>

namespace btm {

VerseChooser::VerseChooser(QtQuick2ApplicationViewer* viewer, BtWindowInterface* bibleVerse)
    : m_viewer(viewer),
      m_gridChooser(0),
      bibleVerse_(bibleVerse),
      m_state(CLOSED ) {
    m_gridChooser = new GridChooser(m_viewer);
    bool ok = connect(m_gridChooser, SIGNAL(accepted(const QString&)),
                      this, SLOT(stringAccepted(const QString&)));
    Q_ASSERT(ok);
}

void VerseChooser::open() {
    m_oldBook = bibleVerse_->getBook();
    m_oldChapter = bibleVerse_->getChapter();
    m_oldVerse = bibleVerse_->getVerse();
    m_state = BOOK;
    QStringList books = bibleVerse_->getBooks();
    m_gridChooser->open(books, m_oldBook, "Book");
}

void VerseChooser::stringAccepted(const QString& value) {
    if (m_state == BOOK) {
        m_newBook = value;
        m_state = CHAPTER;
        bibleVerse_->setBook(value);
        QStringList chapters = bibleVerse_->getChapters();
        m_gridChooser->open(chapters, m_oldChapter, "Chapter");
    }
    else if (m_state == CHAPTER) {
        m_newChapter = value;
        m_state = VERSE;
        bibleVerse_->setChapter(value);
        QStringList verses = bibleVerse_->getVerses();
        m_gridChooser->open(verses, m_oldVerse, "Verse");
    }
    else if (m_state == VERSE) {
        m_newVerse = value;
        m_state = CLOSED;
        bibleVerse_->setVerse(m_newVerse);
    }
}

void VerseChooser::stringCanceled() {
}

} // end namespace
