/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cswordversekey.h"

#include <QDebug>
#include <QStringList>

#include "../drivers/cswordbiblemoduleinfo.h"
#include "../drivers/cswordcommentarymoduleinfo.h"

// Sword includes:
#include <swmodule.h>
#include <localemgr.h>


CSwordVerseKey::CSwordVerseKey(const CSwordModuleInfo *module)
    : CSwordKey(module)
{
    typedef CSwordBibleModuleInfo CSBMI;
    if (const CSBMI *bible = dynamic_cast<const CSBMI*>(module) ) {
        // Copy important settings like versification system
        copyFrom(static_cast<sword::VerseKey*>(bible->module()->getKey()));

        setKey( bible->lowerBound().key() );
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
    typedef CSwordBibleModuleInfo CSBMI;

    Q_ASSERT(newModule);
    if (m_module == newModule) return;
    Q_ASSERT(newModule->type() == CSwordModuleInfo::Bible
             || newModule->type() == CSwordModuleInfo::Commentary);

    m_module = newModule;

    //check if the module contains the key we present
    const CSBMI* bible = dynamic_cast<const CSBMI*>(newModule);

    if (_compare(bible->lowerBound()) < 0) {
        setKey(bible->lowerBound());
    }

    if (_compare(bible->upperBound()) > 0) {
        setKey(bible->upperBound());
    }
}

/** Returns the current book as Text, not as integer. */
QString CSwordVerseKey::book( const QString& newBook ) {
    typedef CSwordBibleModuleInfo CSBMI;
    int min = 0;
    int max = 1;

    const CSBMI *bible = dynamic_cast<const CSBMI*>(module());
    if (bible != 0) {
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
    return QString::null;
}

/** Sets the key we use to the parameter. */
QString CSwordVerseKey::key() const {
    return QString::fromUtf8(getText());
}

const char * CSwordVerseKey::rawKey() const {
    return getText();
}

bool CSwordVerseKey::setKey(const QString &newKey) {
    return setKey(newKey.toUtf8().constData());
}

bool CSwordVerseKey::setKey(const char *newKey) {
    typedef CSwordBibleModuleInfo CSBMI;

    /// \todo Is this check necessary?
    if (newKey) {
        /// \todo Is this check necessary?
        // Check if empty string:
        if (*newKey != '\0') {
            QString newKeyStr = newKey;
            emitBeforeChanged();
            positionFrom(newKey);
        } else {
            const CSwordModuleInfo *m = module();
            if (m->type() == CSwordModuleInfo::Bible) {
                Q_ASSERT(dynamic_cast<const CSBMI*>(m) != 0);
                const CSBMI *bible = static_cast<const CSBMI*>(m);
                emitBeforeChanged();
                positionFrom(bible->lowerBound().key().toUtf8().constData());
            }
        }
    }

    /// \todo Do we ALWAYS need to emit this signal and check for errors?
    emitAfterChanged();
    return !popError();
}

bool CSwordVerseKey::next( const JumpType type ) {
    typedef CSwordBibleModuleInfo CSBMI;

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
            } else if (sword::SWModule * const swModule = m_module->module()) {
                const bool oldStatus = swModule->isSkipConsecutiveLinks();
                swModule->setSkipConsecutiveLinks(true);

                VerseKey * vKey = static_cast<VerseKey *>(swModule->getKey());

                // disable headings for next verse
                bool const oldHeadingsStatus = vKey->isIntros();
                vKey->setIntros(true);
                //don't use setKey(), that would create a new key without Headings set
                vKey->setText(key().toUtf8().constData());

                (*swModule)++;

                vKey = static_cast<VerseKey *>(swModule->getKey());
                vKey->setIntros(oldHeadingsStatus);
                swModule->setSkipConsecutiveLinks(oldStatus);

                if (!swModule->popError()) {
                    setKey(QString::fromUtf8(vKey->getText()));
                } else {
                    //         Verse(Verse()+1);
                    //don't change the key, restore the module's position
                    vKey->setText(key().toUtf8().constData());
                    ret = false;
                    break;
                }
            } else {
                setVerse(getVerse() + 1);
            }

            break;
        }

        default:
            return false;
    }

    const CSBMI *bible = dynamic_cast<const CSBMI*>(module());
    if (bible != 0) {
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
    typedef CSwordBibleModuleInfo CSBMI;

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
            } else if (sword::SWModule * const swModule = m_module->module()) {
                VerseKey * vKey = static_cast<VerseKey *>(swModule->getKey());
                bool const oldHeadingsStatus = vKey->isIntros();
                vKey->setIntros(true);
                vKey->setText(key().toUtf8().constData());

                bool const oldStatus = swModule->isSkipConsecutiveLinks();
                swModule->setSkipConsecutiveLinks(true);
                (*swModule)--;

                vKey = static_cast<VerseKey *>(swModule->getKey());
                vKey->setIntros(oldHeadingsStatus);
                swModule->setSkipConsecutiveLinks(oldStatus);

                if (!swModule->popError()) {
                    /// \warning Weird comment:
                    // don't use fromUtf8:
                    setKey(QString::fromUtf8(vKey->getText()));
                } else {
                    ret = false;
                    //         Verse(Verse()-1);
                    // Restore module's key:
                    vKey->setText(key().toUtf8().constData());
                }
            } else {
                setVerse(getVerse() - 1);
            }

            break;
        }

        default:
            return false;
    }

    const CSBMI *bible = dynamic_cast<const CSBMI*>(module());
    if (bible != 0) {
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
