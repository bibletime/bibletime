/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, https://bibletime.info/
*
* Copyright 1999-2021 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cswordbiblemoduleinfo.h"

#include <memory>
#include <QFile>
#include "../../util/btassert.h"
#include "../managers/cswordbackend.h"
#include "../keys/cswordversekey.h"

// Sword includes:
#include <versekey.h>


CSwordBibleModuleInfo::CSwordBibleModuleInfo(sword::SWModule & module,
                                             CSwordBackend & backend,
                                             ModuleType type)
        : CSwordModuleInfo(module, backend, type)
        , m_boundsInitialized(false)
        , m_lowerBound(nullptr)
        , m_upperBound(nullptr)
{}

CSwordBibleModuleInfo::~CSwordBibleModuleInfo() noexcept = default;

void CSwordBibleModuleInfo::initBounds() const {
    /// \todo The fields calculated by this method could be cached to disk.

    BT_ASSERT(!m_boundsInitialized);

    auto & m = swordModule();
    const bool oldStatus = m.isSkipConsecutiveLinks();
    m.setSkipConsecutiveLinks(true);

    sword::VerseKey const * const key =
        static_cast<sword::VerseKey *>(m.getKey());
    m_lowerBound.setVersification(key->getVersificationSystem());
    m_upperBound.setVersification(key->getVersificationSystem());

    m.setPosition(sword::TOP); // position to first entry
    m_hasOT = (key->getTestament() == 1);
    m_lowerBound.setKey(key->getText());

    m.setPosition(sword::BOTTOM);
    m_hasNT = (key->getTestament() == 2);
    m_upperBound.setKey(key->getText());

    m.setSkipConsecutiveLinks(oldStatus);
    m_boundsInitialized = true;
}


/** Returns the books available in this module */
QStringList const & CSwordBibleModuleInfo::books() const {
    {
        CSwordBackend & b = backend();
        if (m_cachedLocale != b.booknameLanguage()) {
            // Reset the booklist because the locale has changed
            m_cachedLocale = b.booknameLanguage();
            m_bookList.reset();
        }
    }

    if (!m_bookList) {
        m_bookList.emplace();

        if (!m_boundsInitialized)
            initBounds();

        CSwordVerseKey key(this);
        key.setKey(m_lowerBound.key());
        do {
            m_bookList->append(key.bookName());
        } while (key.next(CSwordVerseKey::JumpType::UseBook) &&
                 key <= m_upperBound);
    }

    return *m_bookList;
}

unsigned int CSwordBibleModuleInfo::chapterCount(const unsigned int book) const {
    int result = 0;

    std::unique_ptr<sword::VerseKey> key(
            static_cast<sword::VerseKey *>(swordModule().createKey()));
    key->setPosition(sword::TOP);

    // works for old and new versions
    key->setBook(book);
    key->setPosition(sword::MAXCHAPTER);
    result = key->getChapter();

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

    std::unique_ptr<sword::VerseKey> key(
            static_cast<sword::VerseKey *>(swordModule().createKey()));
    key->setPosition(sword::TOP);

    // works for old and new versions
    key->setBook(book);
    key->setChapter(chapter);
    key->setPosition(sword::MAXVERSE);
    result = key->getVerse();

    return result;
}

unsigned int CSwordBibleModuleInfo::verseCount(const QString &book,
                                               const unsigned int chapter) const
{
    return verseCount(bookNumber(book), chapter);
}

unsigned int CSwordBibleModuleInfo::bookNumber(const QString &book) const {
    unsigned int bookNumber = 0;

    std::unique_ptr<sword::VerseKey> key(
            static_cast<sword::VerseKey *>(swordModule().createKey()));
    key->setPosition(sword::TOP);

    key->setBookName(book.toUtf8().constData());

    bookNumber = ((key->getTestament() > 1) ? key->BMAX[0] : 0) + key->getBook();

    return bookNumber;
}

CSwordKey * CSwordBibleModuleInfo::createKey() const {
    auto * const key = swordModule().getKey();
    BT_ASSERT(dynamic_cast<sword::VerseKey *>(key));
    return new CSwordVerseKey(static_cast<sword::VerseKey *>(key), this);
}
