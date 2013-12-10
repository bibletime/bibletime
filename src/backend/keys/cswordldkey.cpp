/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2013 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/keys/cswordldkey.h"

#include <QTextCodec>
#include "backend/drivers/cswordlexiconmoduleinfo.h"

// Sword includes:
#include <swmodule.h>
#include <swld.h>
#include <utilstr.h>


CSwordLDKey::CSwordLDKey(const CSwordModuleInfo *module) {
    if ((m_module = dynamic_cast<const CSwordLexiconModuleInfo*>(module))) {
        //    *(m_module->module()) = TOP;
    }

    SWKey::operator= (" ");
}

/** No descriptions */
CSwordLDKey::CSwordLDKey( const CSwordLDKey &k ) : CSwordKey(k), SWKey((const char*)k) {}

/** No descriptions */
CSwordLDKey::CSwordLDKey(const SWKey *k, const CSwordModuleInfo *module)
    : CSwordKey(module), SWKey(*k)
{
    // Intentionally empty
}

/** Clones this object by copying the members. */
CSwordLDKey* CSwordLDKey::copy() const {
    return new CSwordLDKey(*this);
}

/** Sets the module of this key. */
void CSwordLDKey::setModule(const CSwordModuleInfo *newModule) {
    Q_ASSERT(newModule);
    if (m_module == newModule) return;
    Q_ASSERT(newModule->type() == CSwordModuleInfo::Lexicon);

    const QString oldKey = key();
    m_module = newModule;
    setKey(oldKey);
}

QString CSwordLDKey::key() const {
    //return QString::fromUtf8((const char*)*this);
    Q_ASSERT(m_module);

    if (m_module->isUnicode()) {
        return QString::fromUtf8((const char*)*this);
    }
    else {
        return cp1252Codec()->toUnicode((const char*)*this);
    }
}

const char * CSwordLDKey::rawKey() const {
    return getText();
}

bool CSwordLDKey::setKey(const QString &newKey) {
    Q_ASSERT(m_module);

    if (m_module->isUnicode()) {
        return setKey(newKey.toUtf8().constData());
    }
    else {
        return setKey(cp1252Codec()->fromUnicode(newKey).constData());
    }
}


/** Sets the key of this instance */
bool CSwordLDKey::setKey(const char *newKey) {
    Q_ASSERT(newKey);

    if (newKey) {
        SWKey::operator = (newKey); //set the key
        m_module->module()->setKey(this);
        m_module->snap();
    }

    return !popError();
}

/** Uses the parameter to returns the next entry afer this key. */
CSwordLDKey* CSwordLDKey::NextEntry() {
    m_module->module()->setKey(this); // use this key as base for the next one!
    //   m_module->module()->getKey()->setText( (const char*)key().utf8() );

    m_module->module()->setSkipConsecutiveLinks(true);
    ( *( m_module->module() ) )++;
    m_module->module()->setSkipConsecutiveLinks(false);

    setKey(m_module->module()->getKeyText());
    setText(m_module->module()->getKeyText());

    return this;
}

/** Uses the parameter to returns the next entry afer this key. */
CSwordLDKey* CSwordLDKey::PreviousEntry() {
    m_module->module()->setKey(this); // use this key as base for the next one!
    //   m_module->module()->getKey()->setText( (const char*)key().utf8() );

    m_module->module()->setSkipConsecutiveLinks(true);
    ( *( m_module->module() ) )--;
    m_module->module()->setSkipConsecutiveLinks(false);

    setText(m_module->module()->getKeyText());

    return this;
}

