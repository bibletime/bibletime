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
#include "../drivers/cswordcommentarymoduleinfo.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <swmodule.h>
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
    const CSBMI * bible = dynamic_cast<const CSBMI *>(module());
    bool valid = m_valid;
    if (bible != nullptr) {
        // not sure why bounds (testament persistance) check was here, just
        // preserve it to initialize module boundaries
        if (*this < bible->lowerBound() && *this > bible->upperBound()) {
            valid = false;
        }
    }

    if (valid) {
        return QString::fromUtf8(m_key.getBookName());
    }
    return QString();
}

/** Sets the key we use to the parameter. */
QString CSwordVerseKey::key() const {
    return QString::fromUtf8(m_key.isBoundSet()
                             ? m_key.getRangeText()
                             : m_key.getText());
}

QString CSwordVerseKey::normalizedKey() const {
    if (m_key.getLocale() == std::string_view("en"))
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
    qDebug() << "setKey < " << this << newKey;
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

    qDebug() << "setKey > " << this << key() << m_valid;
    return m_valid;
}

bool CSwordVerseKey::next( const JumpType type ) {
    return jump(type, 1);
}

bool CSwordVerseKey::previous( const JumpType type ) {
    return jump(type, -1);
}

bool CSwordVerseKey::jump(const JumpType type, const int amount)
{
    sword::VerseKey verseKey(m_module->swordModule().getKey());
    qDebug() << "\nSTART" << type << verseKey.getText() << key();
    // TODO make thread safe, restore
    m_module->swordModule().setSkipConsecutiveLinks(true);
    verseKey.setAutoNormalize(true);
    verseKey.setIntros(false);
    //don't use setKey(), that would create a new key without Headings set
    verseKey.setText(key().toUtf8().constData());
    qDebug() << "start2" << verseKey.getText() << key();

    for (int i = 0, n = abs(amount), s = amount > 0 ? 1 : -1; i < n; i++) {
        switch (type) {
            case UseBook: {
                verseKey.setBook(verseKey.getBook() + s);
                break;
            }

            case UseChapter: {
                verseKey.setChapter(verseKey.getChapter() + s);
                break;
            }

            case UseVerse: {
                verseKey.setVerse(verseKey.getVerse() + s);
                break;
            }

            default:
                return false;
        }

        if (verseKey.popError()) {
            return false;
        }
    }

    qDebug() << "jumped" << verseKey.getText();

    auto * bible = dynamic_cast<const CSwordBibleModuleInfo *>(module());
    if (verseKey._compare(bible->lowerBound().m_key) < 0 ) {
        setKey(bible->lowerBound().m_key);
        qDebug() << "restore lower" << verseKey.getText();
        return false;
    }

    if (verseKey._compare(bible->upperBound().m_key) > 0 ) {
        setKey(bible->upperBound().m_key);
        qDebug() << "restore upper" << verseKey.getText();
        return false;
    }

    return setKey(QString::fromUtf8(verseKey.getText()));
}
