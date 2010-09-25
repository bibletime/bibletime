/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/drivers/cswordbiblemoduleinfo.h"

#include <QSharedPointer>
#include <QFile>
#include "backend/managers/cswordbackend.h"
#include "backend/keys/cswordversekey.h"

// Sword includes:
#include <versekey.h>


CSwordBibleModuleInfo::CSwordBibleModuleInfo(sword::SWModule *module,
                                             CSwordBackend * const usedBackend,
                                             ModuleType type)
        : CSwordModuleInfo(module, usedBackend, type),
        m_lowerBound(0),
        m_upperBound(0),
        m_bookList(0),
        m_cachedLocale("unknown")
{
    initBounds();
}

CSwordBibleModuleInfo::CSwordBibleModuleInfo(const CSwordBibleModuleInfo &copy) :
        CSwordModuleInfo(copy),
        m_lowerBound(0),
        m_upperBound(0),
        m_bookList(0),
        m_cachedLocale(copy.m_cachedLocale),
        m_hasOT(copy.m_hasOT),
        m_hasNT(copy.m_hasNT)
{
    if (copy.m_bookList) {
        m_bookList = new QStringList();
        *m_bookList = *copy.m_bookList;
    }
    initBounds();
}

void CSwordBibleModuleInfo::initBounds() {
    const bool oldStatus = module()->getSkipConsecutiveLinks();
    module()->setSkipConsecutiveLinks(true);

    module()->setPosition(sword::TOP); // position to first entry
    sword::VerseKey key(module()->KeyText());
    m_hasOT = (key.Testament() == 1);

    module()->setPosition(sword::BOTTOM);
    key = module()->KeyText();
    m_hasNT = (key.Testament() == 2);

    module()->setSkipConsecutiveLinks(oldStatus);

    m_lowerBound.setKey(m_hasOT ? "Genesis 1:1" : "Matthew 1:1");
    m_upperBound.setKey(!m_hasNT ? "Malachi 4:6" : "Revelation of John 22:21");
}


/** Returns the books available in this module */
QStringList *CSwordBibleModuleInfo::books() const {
    if (m_cachedLocale != backend()->booknameLanguage()) { //if the locale has changed
        delete m_bookList;
        m_bookList = 0;
    }

    if (!m_bookList) {
        m_bookList = new QStringList();

        int min = 1; // 1 = OT
        int max = 2; // 2 = NT

        //find out if we have ot and nt, only ot or only nt

        if (m_hasOT && m_hasNT) { //both
            min = 1;
            max = 2;
        }
        else if (m_hasOT && !m_hasNT) { //only OT
            min = 1;
            max = 1;
        }
        else if (!m_hasOT && m_hasNT) { //only NT
            min = 2;
            max = 2;
        }
        else if (!m_hasOT && !m_hasNT) { //somethings wrong here! - no OT and no NT
            qWarning("CSwordBibleModuleInfo (%s) no OT and not NT! Check your config!", module()->Name());
            min = 1;
            max = 0;
        }

        QSharedPointer<sword::VerseKey> key((sword::VerseKey *)module()->CreateKey());
        key->setPosition(sword::TOP);

        for (key->setTestament(min); !key->Error() && key->getTestament() <= max; key->Book(key->Book() + 1)) {
            m_bookList->append( QString::fromUtf8(key->getBookName()) );
        }

        m_cachedLocale = backend()->booknameLanguage();
    }

    return m_bookList;
}

unsigned int CSwordBibleModuleInfo::chapterCount(const unsigned int book) const {
    int result = 0;

    QSharedPointer<sword::VerseKey> key((sword::VerseKey *)module()->CreateKey());
    key->setPosition(sword::TOP);

    // works for old and new versions
    key->Book(book);
    key->setPosition(sword::MAXCHAPTER);
    result = key->Chapter();

    return result;
}

unsigned int CSwordBibleModuleInfo::chapterCount(const QString &book) const {
    return chapterCount(bookNumber(book));
}

/** Returns the number of verses  for the given chapter. */

unsigned int CSwordBibleModuleInfo::verseCount(const unsigned int book,
                                               const unsigned int chapter) const
{
    unsigned int result = 0;

    QSharedPointer<sword::VerseKey> key((sword::VerseKey *)module()->CreateKey());
    key->setPosition(sword::TOP);

    // works for old and new versions
    key->Book(book);
    key->Chapter(chapter);
    key->setPosition(sword::MAXVERSE);
    result = key->Verse();

    return result;
}

unsigned int CSwordBibleModuleInfo::verseCount(const QString &book,
                                               const unsigned int chapter) const
{
    return verseCount(bookNumber(book), chapter);
}

unsigned int CSwordBibleModuleInfo::bookNumber(const QString &book) const {
    unsigned int bookNumber = 0;

    QSharedPointer<sword::VerseKey> key((sword::VerseKey *)module()->CreateKey());
    key->setPosition(sword::TOP);

    key->setBookName(book.toUtf8().constData());

    bookNumber = ((key->getTestament() > 1) ? key->BMAX[0] : 0) + key->Book();

    return bookNumber;
}
