/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/
*
* Copyright 1999-2020 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "cswordversekey.h"

#include <QDebug>
#include <QStringList>
#include "../../util/btassert.h"
#include "../drivers/cswordbiblemoduleinfo.h"
#include "../drivers/cswordcommentarymoduleinfo.h"

// Sword includes:
#include <swmodule.h>
#include <localemgr.h>


CSwordVerseKey::CSwordVerseKey(const CSwordModuleInfo *module)
    : CSwordKey(module)
{
    if(CSwordBibleModuleInfo const * bible =
            dynamic_cast<CSwordBibleModuleInfo const *>(module))
    {
        // Copy important settings like versification system
        copyFrom(bible->module().getKey());
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
    const char * newVersification =
            static_cast<VerseKey *>(bible->module().getKey())->getVersificationSystem();
    bool inVersification = true;

    emitBeforeChanged();

    if(strcmp(getVersificationSystem(), newVersification)) {
        /// Remap key position to new versification
        sword::VerseKey oldKey(*this);

        setVersificationSystem(newVersification);

        positionFrom(oldKey);
        inVersification = !popError();
    }

    m_module = newModule;

    emitAfterChanged();

    if(inVersification) {
        /// Limit to Bible bounds
        if (_compare(bible->lowerBound()) < 0) {
            setKey(bible->lowerBound());
        }
        if (_compare(bible->upperBound()) > 0) {
            setKey(bible->upperBound());
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

    if ((getTestament() >= min + 1) && (getTestament() <= max + 1) && (getBook() <= BMAX[min])) {
        return QString::fromUtf8( getBookName() );
    }

    //return QString::fromUtf8( books[min][0].name ); //return the first book, i.e. Genesis
    return QString();
}

/** Sets the key we use to the parameter. */
QString CSwordVerseKey::key() const {
    return QString::fromUtf8(isBoundSet() ? getRangeText() : getText());
}

const char * CSwordVerseKey::rawKey() const {
    return getText();
}

bool CSwordVerseKey::setKey(const QString &newKey) {
    return setKey(newKey.toUtf8().constData());
}

bool CSwordVerseKey::setKey(const char *newKey) {
    emitBeforeChanged();

    if(QByteArray(newKey).contains('-')) {
        VerseKey vk(newKey, newKey, getVersificationSystem());
        setLowerBound(vk.getLowerBound());
        setUpperBound(vk.getUpperBound());
        setPosition(sword::TOP);
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

            if ((currentTestament == 2) && (currentBook >= BMAX[currentTestament-1])) { //Revelation, i.e. end of navigation
                return false;
            }
            else if ((currentTestament == 1) && (currentBook >= BMAX[currentTestament-1])) { //Malachi, switch to the NT
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

                VerseKey * vKey = static_cast<VerseKey *>(m.getKey());

                // disable headings for next verse
                bool const oldHeadingsStatus = vKey->isIntros();
                vKey->setIntros(true);
                //don't use setKey(), that would create a new key without Headings set
                vKey->setText(key().toUtf8().constData());

                m++;

                vKey = static_cast<VerseKey *>(m.getKey());
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
        if (_compare(bible->lowerBound()) < 0 ) {
            emitBeforeChanged();
            setKey(bible->lowerBound());
            ret = false;
        }

        if (_compare(bible->upperBound()) > 0 ) {
            emitBeforeChanged();
            setKey(bible->upperBound());
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
                setBook(BMAX[0]);
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
                VerseKey * vKey = static_cast<VerseKey *>(m.getKey());
                bool const oldHeadingsStatus = vKey->isIntros();
                vKey->setIntros(true);
                vKey->setText(key().toUtf8().constData());

                bool const oldStatus = m.isSkipConsecutiveLinks();
                m.setSkipConsecutiveLinks(true);
                m--;

                vKey = static_cast<VerseKey *>(m.getKey());
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
        if (_compare(bible->lowerBound()) < 0 ) {
            emitBeforeChanged();
            setKey(bible->lowerBound());
            ret = false;
        }

        if (_compare(bible->upperBound()) > 0 ) {
            emitBeforeChanged();
            setKey(bible->upperBound());
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
