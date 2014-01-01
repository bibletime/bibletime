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

#include "versechooser.h"

#include "backend/keys/cswordversekey.h"
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordmoduleinfo.h"
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
      m_key(0),
      m_state(CLOSED ) {
    m_gridChooser = new GridChooser(m_viewer);
    bool ok = connect(m_gridChooser, SIGNAL(accepted(const QString&)),
                      this, SLOT(stringAccepted(const QString&)));
    Q_ASSERT(ok);
}

void VerseChooser::open(CSwordVerseKey* key) {
    if (key == 0)
        return;
    m_key = key;
    m_oldBook = getBook();
    m_oldChapter = getChapter();
    m_oldVerse = getVerse();
    m_state = BOOK;
    QStringList books = getBooks();
    m_gridChooser->open(books, m_oldBook, "Book");
}

void VerseChooser::stringAccepted(const QString& value) {
    if (m_state == BOOK) {
        m_newBook = value;
        m_state = CHAPTER;
        setBook(value);
        QStringList chapters = getChapters();
        m_gridChooser->open(chapters, m_oldChapter, "Chapter");
    }
    else if (m_state == CHAPTER) {
        m_newChapter = value;
        m_state = VERSE;
        setChapter(value);
        QStringList verses = getVerses();
        m_gridChooser->open(verses, m_oldVerse, "Verse");
    }
    else if (m_state == VERSE) {
        m_newVerse = value;
        m_state = CLOSED;
        setVerse(m_newVerse);
    }
}

void VerseChooser::stringCanceled() {
}

QString VerseChooser::getBook() const {
    QString book;
    if (m_key)
        book = m_key->book();
    return book;
}

void VerseChooser::setBook(const QString& book) {
    if (m_key) {
        m_key->book(book);
    }
}

QString VerseChooser::getChapter() const {
    QString chapter;
    if (m_key)
        chapter = QString::number(m_key->getChapter());
    return chapter;
}

void VerseChooser::setChapter(const QString& chapter)  {
    if (m_key) {
        int iChapter = chapter.toInt();
        m_key->setChapter(iChapter);
    }
}

QString VerseChooser::getVerse() const {
    QString verse;
    if (m_key) {
        verse = QString::number(m_key->getVerse());
    }
    return verse;
}

void VerseChooser::setVerse(const QString& verse) {
    if (m_key) {
        int iVerse = verse.toInt();
        m_key->setVerse(iVerse);
        emit referenceChanged();
    }
}

QStringList VerseChooser::getBooks() const {
    QStringList books;
    const CSwordModuleInfo* module = m_key->module();
    const CSwordBibleModuleInfo* bibleModule = qobject_cast<const CSwordBibleModuleInfo*>(module);
    if (bibleModule)
        books = *bibleModule->books();
    return books;
}

QStringList VerseChooser::getChapters() const {
    QStringList chapters;
    const CSwordModuleInfo* module = m_key->module();
    const CSwordBibleModuleInfo* m = qobject_cast<const CSwordBibleModuleInfo*>(module);

    QString book = m_key->book();
    int count = m->chapterCount(book);
    for (int i = 1; i <= count; i++) {
        chapters << QString::number(i);
    }
    return chapters;
}

QStringList VerseChooser::getVerses() const {
    QStringList verses;
    const CSwordModuleInfo* module = m_key->module();
    const CSwordBibleModuleInfo* m = qobject_cast<const CSwordBibleModuleInfo*>(module);
    QString book = m_key->book();
    int chapter = m_key->getChapter();
    int count = m->verseCount(book,chapter);
    for (int i = 1; i <= count; i++) {
        verses << QString::number(i);
    }
    return verses;
}



} // end namespace
