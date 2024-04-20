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

#include "cswordversekey.h"

#include <QDebug>
#include <QStringList>
#include <string_view>
#include "../../util/btassert.h"
#include "../drivers/cswordbiblemoduleinfo.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <swmodule.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


CSwordVerseKey::CSwordVerseKey(const CSwordModuleInfo *module)
    : CSwordKey(module)
{
    if(CSwordBibleModuleInfo const * bible =
            dynamic_cast<CSwordBibleModuleInfo const *>(module))
    {
        // Copy important settings like versification system
        m_key.copyFrom(bible->swordModule().getKey());
        setKey(bible->lowerBound().key());
    }
    m_key.setAutoNormalize(true);
}

CSwordVerseKey::CSwordVerseKey(CSwordVerseKey const & copy)
    : CSwordKey(copy)
    , m_key(copy.m_key)
{ m_key.setAutoNormalize(true); }

CSwordVerseKey::CSwordVerseKey(sword::VerseKey const * k,
                               CSwordModuleInfo const * module)
    : CSwordKey(module)
    , m_key(*k)
{}

CSwordVerseKey::~CSwordVerseKey() { delete m_afterChangedSignaller.data(); }

sword::SWKey const & CSwordVerseKey::asSwordKey() const noexcept
{ return m_key; }

/** Clones this object. */
CSwordVerseKey * CSwordVerseKey::copy() const
{ return new CSwordVerseKey(*this); }

/** Sets the module for this key */
void CSwordVerseKey::setModule(const CSwordModuleInfo *newModule) {
    BT_ASSERT(newModule);
    if (m_module == newModule) return;
    BT_ASSERT(newModule->type() == CSwordModuleInfo::Bible ||
             newModule->type() == CSwordModuleInfo::Commentary);

    CSwordBibleModuleInfo const * bible = static_cast<CSwordBibleModuleInfo const *>(newModule);
    const char * newVersification =
            static_cast<sword::VerseKey *>(
                bible->swordModule().getKey())->getVersificationSystem();
    bool inVersification = true;

    if (strcmp(m_key.getVersificationSystem(), newVersification)) {
        /// Remap key position to new versification
        sword::VerseKey oldKey(m_key);

        m_key.setVersificationSystem(newVersification);

        m_key.positionFrom(oldKey);
        inVersification = !m_key.popError();
    }

    m_module = newModule;

    emitAfterChanged();

    if(inVersification) {
        /// Limit to Bible bounds
        if (m_key._compare(bible->lowerBound().m_key) < 0) {
            setKey(bible->lowerBound().m_key);
        }
        if (m_key._compare(bible->upperBound().m_key) > 0) {
            setKey(bible->upperBound().m_key);
        }
    }

    m_valid = inVersification;
}

char CSwordVerseKey::bibleBook() const
{ return ((testament() > 1) ? m_key.BMAX[0] : 0) + book(); }

CSwordVerseKey CSwordVerseKey::lowerBound() const
{ return {&m_key.getLowerBound(), module()}; }

void CSwordVerseKey::setLowerBound(CSwordVerseKey const & bound)
{ m_key.setLowerBound(bound.m_key); }

CSwordVerseKey CSwordVerseKey::upperBound() const
{ return {&m_key.getUpperBound(), module()}; }

void CSwordVerseKey::setUpperBound(CSwordVerseKey const & bound)
{ m_key.setUpperBound(bound.m_key); }

/** Returns the current book as Text, not as integer. */
QString CSwordVerseKey::bookName() const {
    using CSBMI = CSwordBibleModuleInfo;
    int min = 0;
    int max = 1;

    const CSBMI *bible = dynamic_cast<const CSBMI*>(module());
    if (bible != nullptr) {
        const bool hasOT = bible->hasOldTestament();
        const bool hasNT = bible->hasNewTestament();

        if (hasOT && hasNT) {
            min = 0;
            max = 1;
        }
        else if (hasOT && !hasNT) {
            min = 0;
            max = 0;
        }
        else if (!hasOT && hasNT) {
            min = 1;
            max = 1;
        }
        else if (!hasOT && !hasNT) {
            min = 0;
            max = -1; //no loop
        }
    }

    if ((m_key.getTestament() >= min + 1) && (m_key.getTestament() <= max + 1) && (m_key.getBook() <= m_key.BMAX[min])) {
        return QString::fromUtf8(m_key.getBookName());
    }

    //return QString::fromUtf8( books[min][0].name ); //return the first book, i.e. Genesis
    return QString();
}

/** Sets the key we use to the parameter. */
QString CSwordVerseKey::key() const {
    return QString::fromUtf8(m_key.isBoundSet()
                             ? m_key.getRangeText()
                             : m_key.getText());
}

QString CSwordVerseKey::normalizedKey() const {
    using namespace std::string_view_literals;
    if (m_key.getLocale() == "en"sv)
        return key();
    sword::VerseKey clone(m_key);
    clone.setLocale("en");
    return QString::fromUtf8(clone.isBoundSet()
                             ? clone.getRangeText()
                             : clone.getText());
}

const char * CSwordVerseKey::rawKey() const { return m_key.getText(); }

bool CSwordVerseKey::setKey(const QString &newKey) {
    return setKey(newKey.toUtf8().constData());
}

bool CSwordVerseKey::setKey(const char *newKey) {
    if(QByteArray(newKey).contains('-')) {
        sword::VerseKey vk(newKey, newKey, m_key.getVersificationSystem());
        m_key.setLowerBound(vk.getLowerBound());
        m_key.setUpperBound(vk.getUpperBound());
        m_key.setPosition(sword::TOP);
    } else {
        m_key.clearBounds();
        m_key.positionFrom(newKey);
    }

    m_valid = !m_key.popError();

    emitAfterChanged(); /// \todo Do we ALWAYS need to emit this signal

    return m_valid;
}

bool CSwordVerseKey::next( const JumpType type ) {
    using CSBMI = CSwordBibleModuleInfo;

    m_key.popError(); //clear Error status
    bool ret = true;

    switch (type) {

        case UseBook: {
            const int currentTestament = m_key.getTestament();
            const int currentBook = m_key.getBook();

            if ((currentTestament == 2) && (currentBook >= m_key.BMAX[currentTestament-1])) { //Revelation, i.e. end of navigation
                return false;
            }
            else if ((currentTestament == 1) && (currentBook >= m_key.BMAX[currentTestament-1])) { //Malachi, switch to the NT
                m_key.setTestament(currentTestament + 1);
                m_key.setBook(1);
            }
            else {
                m_key.setBook(m_key.getBook() + 1);
            }
            break;
        }

        case UseChapter: {
            m_key.setChapter(m_key.getChapter() + 1);
            break;
        }

        case UseVerse: {
            if (!m_module) {
                m_key.setVerse(m_key.getVerse() + 1);
            } else {
                auto & m = m_module->swordModule();
                const bool oldStatus = m.isSkipConsecutiveLinks();
                m.setSkipConsecutiveLinks(true);

                auto * vKey = static_cast<sword::VerseKey *>(m.getKey());

                // disable headings for next verse
                bool const oldHeadingsStatus = vKey->isIntros();
                vKey->setIntros(true);
                //don't use setKey(), that would create a new key without Headings set
                vKey->setText(key().toUtf8().constData());

                m++;

                vKey = static_cast<sword::VerseKey *>(m.getKey());
                vKey->setIntros(oldHeadingsStatus);
                m.setSkipConsecutiveLinks(oldStatus);

                if (!m.popError()) {
                    setKey(QString::fromUtf8(vKey->getText()));
                } else {
                    //         Verse(Verse()+1);
                    //don't change the key, restore the module's position
                    vKey->setText(key().toUtf8().constData());
                    ret = false;
                    break;
                }
            }

            break;
        }

        default:
            return false;
    }

    const CSBMI *bible = dynamic_cast<const CSBMI*>(module());
    if (bible != nullptr) {
        if (m_key._compare(bible->lowerBound().m_key) < 0 ) {
            setKey(bible->lowerBound().m_key);
            ret = false;
        }

        if (m_key._compare(bible->upperBound().m_key) > 0 ) {
            setKey(bible->upperBound().m_key);
            ret = false;
        }

        emitAfterChanged();
        return ret;
    }
    else if (m_key.popError()) { //we have no module, so take care of VerseKey::Error()
        return false;
    }

    emitAfterChanged();
    return ret;
}

bool CSwordVerseKey::previous( const JumpType type ) {
    using CSBMI = CSwordBibleModuleInfo;

    bool ret = true;

    switch (type) {

        case UseBook: {
            if ((m_key.getBook() == 1) && (m_key.getTestament() == 1)) { //Genesis
                return false;
            }
            else if ((m_key.getBook() == 1) && (m_key.getTestament() == 2)) { //Matthew
                m_key.setTestament(1);
                m_key.setBook(m_key.BMAX[0]);
            }
            else {
                m_key.setBook(m_key.getBook() - 1);
            }

            break;
        }

        case UseChapter: {
            m_key.setChapter(m_key.getChapter() - 1);
            break;
        }

        case UseVerse: {
            if (!m_module) {
                m_key.setVerse(m_key.getVerse() - 1);
            } else {
                auto & m = m_module->swordModule();
                auto * vKey = static_cast<sword::VerseKey *>(m.getKey());
                bool const oldHeadingsStatus = vKey->isIntros();
                vKey->setIntros(true);
                vKey->setText(key().toUtf8().constData());

                bool const oldStatus = m.isSkipConsecutiveLinks();
                m.setSkipConsecutiveLinks(true);
                m--;

                vKey = static_cast<sword::VerseKey *>(m.getKey());
                vKey->setIntros(oldHeadingsStatus);
                m.setSkipConsecutiveLinks(oldStatus);

                if (!m.popError()) {
                    /// \warning Weird comment:
                    // don't use fromUtf8:
                    setKey(QString::fromUtf8(vKey->getText()));
                } else {
                    ret = false;
                    //         Verse(Verse()-1);
                    // Restore module's key:
                    vKey->setText(key().toUtf8().constData());
                }
            }

            break;
        }

        default:
            return false;
    }

    const CSBMI *bible = dynamic_cast<const CSBMI*>(module());
    if (bible != nullptr) {
        if (m_key._compare(bible->lowerBound().m_key) < 0 ) {
            setKey(bible->lowerBound().m_key);
            ret = false;
        }

        if (m_key._compare(bible->upperBound().m_key) > 0 ) {
            setKey(bible->upperBound().m_key);
            ret = false;
        }

        emitAfterChanged();
        return ret;
    }
    else if (m_key.popError()) {
        return false;
    }

    emitAfterChanged();
    return ret;
}

const BtSignal * CSwordVerseKey::afterChangedSignaller() {
    if (m_afterChangedSignaller.isNull())
        m_afterChangedSignaller = new BtSignal();

    return m_afterChangedSignaller;
}


void CSwordVerseKey::emitAfterChanged() {
    if (!m_afterChangedSignaller.isNull())
        m_afterChangedSignaller->emitSignal();
}
