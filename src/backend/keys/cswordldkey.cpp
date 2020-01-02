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

#include "cswordldkey.h"

#include <QTextCodec>
#include "../../util/btassert.h"
#include "../drivers/cswordlexiconmoduleinfo.h"

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
CSwordLDKey::CSwordLDKey(CSwordLDKey const & k)
    : CSwordKey(k)
    , SWKey(k.getText())
{}

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
        return QString::fromUtf8(getText());
    } else {
        return cp1252Codec()->toUnicode(getText());
    }
}

const char * CSwordLDKey::rawKey() const {
    return getText();
}

bool CSwordLDKey::setKey(const QString &newKey) {
    BT_ASSERT(m_module);

    if (m_module->isUnicode()) {
        return setKey(newKey.toUtf8().constData());
    }
    else {
        return setKey(cp1252Codec()->fromUnicode(newKey).constData());
    }
}


/** Sets the key of this instance */
bool CSwordLDKey::setKey(const char *newKey) {
    BT_ASSERT(newKey);

    if (newKey) {
        SWKey::operator = (newKey); //set the key
        m_module->module().setKey(this);
        m_module->snap();
    }

    return !popError();
}

/** Uses the parameter to returns the next entry afer this key. */
CSwordLDKey* CSwordLDKey::NextEntry() {
    auto & m = m_module->module();
    m.setKey(this); // use this key as base for the next one!
    //   m.getKey()->setText( (const char*)key().utf8() );

    m.setSkipConsecutiveLinks(true);
    m++;
    m.setSkipConsecutiveLinks(false);

    setKey(m.getKeyText());
    setText(m.getKeyText());

    return this;
}

/** Uses the parameter to returns the next entry afer this key. */
CSwordLDKey* CSwordLDKey::PreviousEntry() {
    auto & m = m_module->module();
    m.setKey(this); // use this key as base for the next one!
    //   m.getKey()->setText( (const char*)key().utf8() );

    m.setSkipConsecutiveLinks(true);
    m--;
    m.setSkipConsecutiveLinks(false);

    setText(m.getKeyText());

    return this;
}

