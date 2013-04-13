
#include "versechooser.h"

#include "qtquick2applicationviewer.h"

#include <cmath>
#include <QQuickItem>
#include <QQmlProperty>
#include <QQmlContext>
#include <QDebug>
#include <QQmlProperty>
#include <QCoreApplication>
#include "btwindowinterface.h"
#include "gridchooser.h"

namespace btm {

VerseChooser::VerseChooser(QtQuick2ApplicationViewer* viewer, BtWindowInterface* bibleVerse)
    : viewer_(viewer),
      gridChooser_(0),
      bibleVerse_(bibleVerse),
      state_(CLOSED ) {
    gridChooser_ = new GridChooser(viewer_);
    bool ok = connect(gridChooser_, SIGNAL(accepted(const QString&)),
                      this, SLOT(stringAccepted(const QString&)));
    Q_ASSERT(ok);
}

void VerseChooser::open() {
    oldBook_ = bibleVerse_->getBook();
    oldChapter_ = bibleVerse_->getChapter();
    oldVerse_ = bibleVerse_->getVerse();
    state_ = BOOK;
    QStringList books = bibleVerse_->getBooks();
    gridChooser_->open(books, oldBook_);
}

void VerseChooser::stringAccepted(const QString& value) {
    if (state_ == BOOK) {
        newBook_ = value;
        state_ = CHAPTER;
        bibleVerse_->setBook(value);
        QStringList chapters = bibleVerse_->getChapters();
        gridChooser_->open(chapters, oldChapter_);
    }
    else if (state_ == CHAPTER) {
        newChapter_ = value;
        state_ = VERSE;
        bibleVerse_->setChapter(value);
        QStringList verses = bibleVerse_->getVerses();
        gridChooser_->open(verses, oldVerse_);
    }
    else if (state_ == VERSE) {
        newVerse_ = value;
        state_ = CLOSED;
        bibleVerse_->setVerse(newVerse_);
    }
}

void VerseChooser::stringCanceled() {
}

} // end namespace
