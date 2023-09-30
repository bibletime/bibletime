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

#include "cswordldkey.h"

#include <QByteArray>
#include "../../util/btassert.h"
#include "../../util/cp1252.h"
#include "../drivers/cswordmoduleinfo.h"

// Sword includes:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wextra-semi"
#pragma GCC diagnostic ignored "-Wsuggest-override"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsuggest-destructor-override"
#endif
#include <swkey.h>
#include <swmodule.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#pragma GCC diagnostic pop


CSwordLDKey::CSwordLDKey(CSwordModuleInfo const * module)
    : CSwordKey(module)
{ m_key = " "; }

/** No descriptions */
CSwordLDKey::CSwordLDKey(CSwordLDKey const & k)
    : CSwordKey(k)
    , m_key(k.m_key.getText())
{}

/** No descriptions */
CSwordLDKey::CSwordLDKey(const sword::SWKey *k, const CSwordModuleInfo *module)
    : CSwordKey(module)
    , m_key(*k)
{}

sword::SWKey const & CSwordLDKey::asSwordKey() const noexcept { return m_key; }

/** Clones this object by copying the members. */
CSwordLDKey* CSwordLDKey::copy() const {
    return new CSwordLDKey(*this);
}

/** Sets the module of this key. */
void CSwordLDKey::setModule(const CSwordModuleInfo *newModule) {
    BT_ASSERT(newModule);
    if (m_module == newModule) return;
    BT_ASSERT(newModule->type() == CSwordModuleInfo::Lexicon);

    const QString oldKey = key();
    m_module = newModule;
    setKey(oldKey);
}

QString CSwordLDKey::key() const {
    //return QString::fromUtf8((const char*)*this);
    BT_ASSERT(m_module);

    if (m_module->isUnicode()) {
        return QString::fromUtf8(m_key.getText());
    } else {
        return util::cp1252::toUnicode(m_key.getText());
    }
}

const char * CSwordLDKey::rawKey() const {
    return m_key.getText();
}

bool CSwordLDKey::setKey(const QString &newKey) {
    BT_ASSERT(m_module);

    if (m_module->isUnicode()) {
        return setKey(newKey.toUtf8().constData());
    }
    else {
        return setKey(util::cp1252::fromUnicode(newKey).constData());
    }
}


/** Sets the key of this instance */
bool CSwordLDKey::setKey(const char *newKey) {
    BT_ASSERT(newKey);

    if (newKey) {
        m_key = newKey; //set the key
        m_module->swordModule().setKey(&m_key);
        m_module->snap();
    }

    return !m_key.popError();
}

/** Uses the parameter to returns the next entry afer this key. */
CSwordLDKey* CSwordLDKey::NextEntry() {
    auto & m = m_module->swordModule();
    m.setKey(&m_key); // use this key as base for the next one!
    //   m.getKey()->setText( (const char*)key().utf8() );

    m.setSkipConsecutiveLinks(true);
    m++;
    m.setSkipConsecutiveLinks(false);

    setKey(m.getKeyText());
    m_key.setText(m.getKeyText());

    return this;
}

/** Uses the parameter to returns the next entry afer this key. */
CSwordLDKey* CSwordLDKey::PreviousEntry() {
    auto & m = m_module->swordModule();
    m.setKey(&m_key); // use this key as base for the next one!
    //   m.getKey()->setText( (const char*)key().utf8() );

    m.setSkipConsecutiveLinks(true);
    m--;
    m.setSkipConsecutiveLinks(false);

    m_key.setText(m.getKeyText());

    return this;
}

