/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cswordversekey.h"

#include <QDebug>
#include <QStringList>
#include <swordxx/localemgr.h>
#include <swordxx/swmodule.h>
#include "../../util/btassert.h"
#include "../drivers/cswordbiblemoduleinfo.h"
#include "../drivers/cswordcommentarymoduleinfo.h"


CSwordVerseKey::CSwordVerseKey(const CSwordModuleInfo *module)
    : CSwordKey(module)
{
    if(CSwordBibleModuleInfo const * bible =
            dynamic_cast<CSwordBibleModuleInfo const *>(module))
    {
        // Copy important settings like versification system
        copyFrom(*bible->module().getKey());
        setKey(bible->lowerBound().key());
    }
    this->VerseKey::setAutoNormalize(true);
}

CSwordVerseKey::CSwordVerseKey( const CSwordVerseKey& k ) : CSwordKey(k), VerseKey(k) {
    this->VerseKey::setAutoNormalize(true);
}

CSwordVerseKey::CSwordVerseKey(const VerseKey *k,
                               const CSwordModuleInfo *module)
    : CSwordKey(module), VerseKey(*k)
{
    // Intentionally empty
}

/** Clones this object. */
CSwordKey* CSwordVerseKey::copy() const {
    return new CSwordVerseKey(*this);
}

/** Sets the module for this key */
void CSwordVerseKey::setModule(const CSwordModuleInfo *newModule) {
    BT_ASSERT(newModule);
    if (m_module == newModule) return;
    BT_ASSERT(newModule->type() == CSwordModuleInfo::Bible ||
             newModule->type() == CSwordModuleInfo::Commentary);

    CSwordBibleModuleInfo const * bible = static_cast<CSwordBibleModuleInfo const *>(newModule);
    auto const & newVersification =
            bible->module().getKeyAs<VerseKey>()->getVersificationSystem();
    bool inVersification = true;

    emitBeforeChanged();

    if(getVersificationSystem() != newVersification) {
        /// Remap key position to new versification
        swordxx::VerseKey oldKey(*this);

        setVersificationSystem(newVersification.c_str());

        positionFrom(oldKey);
        inVersification = !popError();
    }

    m_module = newModule;

    emitAfterChanged();

    if(inVersification) {
        /// Limit to Bible bounds
        if (compare_(bible->lowerBound()) < 0) {
            setKey(bible->lowerBound().getText().c_str());
        }
        if (compare_(bible->upperBound()) > 0) {
            setKey(bible->upperBound().getText().c_str());
        }
    }

    m_valid = inVersification;
}

/** Returns the current book as Text, not as integer. */
QString CSwordVerseKey::book( const QString& newBook ) {
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

    if (!newBook.isEmpty()) {
        setBookName(newBook.toUtf8().constData());
    }

    if ((getTestament() >= min + 1) && (getTestament() <= max + 1) && (getBook() <= m_BMAX[min])) {
        return QString::fromStdString(getBookName());
    }

    //return QString::fromUtf8( books[min][0].name ); //return the first book, i.e. Genesis
    return QString::null;
}

/** Sets the key we use to the parameter. */
QString CSwordVerseKey::key() const {
    return QString::fromStdString(isBoundSet() ? getRangeText() : getText());
}

std::string CSwordVerseKey::rawKey() const { return getText(); }

bool CSwordVerseKey::setKey(const QString &newKey) {
    return setKey(newKey.toUtf8().constData());
}

bool CSwordVerseKey::setKey(const char *newKey) {
    emitBeforeChanged();

    if(QByteArray(newKey).contains('-')) {
        VerseKey vk(newKey, newKey, getVersificationSystem().c_str());
        setLowerBoundKey(vk.lowerBoundKey());
        setUpperBoundKey(vk.upperBoundKey());
        positionToTop();
    } else {
        clearBounds();
        positionFrom(newKey);
    }

    m_valid = !popError();

    emitAfterChanged(); /// \todo Do we ALWAYS need to emit this signal

    return m_valid;
}

bool CSwordVerseKey::next( const JumpType type ) {
    using CSBMI = CSwordBibleModuleInfo;

    popError(); //clear Error status
    bool ret = true;

    switch (type) {

        case UseBook: {
            const int currentTestament = getTestament();
            const int currentBook = getBook();

            if ((currentTestament == 2) && (currentBook >= m_BMAX[currentTestament-1])) { //Revelation, i.e. end of navigation
                return false;
            }
            else if ((currentTestament == 1) && (currentBook >= m_BMAX[currentTestament-1])) { //Malachi, switch to the NT
                setTestament(currentTestament + 1);
                setBook(1);
            }
            else {
                setBook(getBook() + 1);
            }
            break;
        }

        case UseChapter: {
            setChapter(getChapter() + 1);
            break;
        }

        case UseVerse: {
            if (!m_module) {
                setVerse(getVerse() + 1);
            } else {
                auto & m = m_module->module();
                const bool oldStatus = m.isSkipConsecutiveLinks();
                m.setSkipConsecutiveLinks(true);

                auto vKey(m.getKeyAs<VerseKey>());

                // disable headings for next verse
                bool const oldHeadingsStatus = vKey->isIntros();
                vKey->setIntros(true);
                //don't use setKey(), that would create a new key without Headings set
                vKey->setText(key().toUtf8().constData());

                m.increment();

                vKey = m.getKeyAs<VerseKey>();
                vKey->setIntros(oldHeadingsStatus);
                m.setSkipConsecutiveLinks(oldStatus);

                if (!m.popError()) {
                    setKey(QString::fromStdString(vKey->getText()));
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
        if (compare_(bible->lowerBound()) < 0 ) {
            emitBeforeChanged();
            setKey(bible->lowerBound().getText().c_str());
            ret = false;
        }

        if (compare_(bible->upperBound()) > 0 ) {
            emitBeforeChanged();
            setKey(bible->upperBound().getText().c_str());
            ret = false;
        }

        emitAfterChanged();
        return ret;
    }
    else if (popError()) { //we have no module, so take care of VerseKey::Error()
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
            if ((getBook() == 1) && (getTestament() == 1)) { //Genesis
                return false;
            }
            else if ((getBook() == 1) && (getTestament() == 2)) { //Matthew
                setTestament(1);
                setBook(m_BMAX[0]);
            }
            else {
                setBook(getBook() - 1);
            }

            break;
        }

        case UseChapter: {
            setChapter(getChapter() - 1);
            break;
        }

        case UseVerse: {
            if (!m_module) {
                setVerse(getVerse() - 1);
            } else {
                auto & m = m_module->module();
                auto vKey(m.getKeyAs<VerseKey>());
                bool const oldHeadingsStatus = vKey->isIntros();
                vKey->setIntros(true);
                vKey->setText(key().toUtf8().constData());

                bool const oldStatus = m.isSkipConsecutiveLinks();
                m.setSkipConsecutiveLinks(true);
                m.decrement();

                vKey = m.getKeyAs<VerseKey>();
                vKey->setIntros(oldHeadingsStatus);
                m.setSkipConsecutiveLinks(oldStatus);

                if (!m.popError()) {
                    /// \warning Weird comment:
                    // don't use fromUtf8:
                    setKey(QString::fromStdString(vKey->getText()));
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
        if (compare_(bible->lowerBound()) < 0 ) {
            emitBeforeChanged();
            setKey(bible->lowerBound().getText().c_str());
            ret = false;
        }

        if (compare_(bible->upperBound()) > 0 ) {
            emitBeforeChanged();
            setKey(bible->upperBound().getText().c_str());
            ret = false;
        }

        emitAfterChanged();
        return ret;
    }
    else if (popError()) {
        return false;
    }

    emitAfterChanged();
    return ret;
}
