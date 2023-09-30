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

#include "cswordtreekey.h"

#include <QByteArray>
#include "../../util/btassert.h"
#include "../../util/cp1252.h"
#include "../drivers/cswordbookmoduleinfo.h"
#include "../drivers/cswordmoduleinfo.h"
#include "cswordkey.h"

// Sword includes:
#include <treekeyidx.h>


CSwordTreeKey::CSwordTreeKey(CSwordTreeKey const & copy)
    : CSwordKey(copy)
    , m_key(copy.m_key)
{}

CSwordTreeKey::CSwordTreeKey(sword::TreeKeyIdx const * k,
                             CSwordModuleInfo const * module)
    : CSwordKey(module)
    , m_key(*k)
{}

sword::TreeKeyIdx const & CSwordTreeKey::asSwordKey() const noexcept
{ return m_key; }

CSwordTreeKey* CSwordTreeKey::copy() const {
    return new CSwordTreeKey(*this);
}

/** Sets the key of this instance */
QString CSwordTreeKey::key() const {
    //return getTextUnicode();
    BT_ASSERT(m_module);
    if (m_module->isUnicode()) {
        return QString::fromUtf8(m_key.getText());
    }
    else {
        return util::cp1252::toUnicode(m_key.getText());
    }
}

const char * CSwordTreeKey::rawKey() const { return m_key.getText(); }

bool CSwordTreeKey::setKey(const QString &newKey) {
    //return key( newKey.toLocal8Bit().constData() );
    //return key(m_module->getTextCodec()->fromUnicode(newKey).constData());
    BT_ASSERT(m_module);
    if (m_module->isUnicode()) {
        return setKey(newKey.toUtf8().constData());
    }
    else {
        return setKey(util::cp1252::fromUnicode(newKey).constData());
    }
}

bool CSwordTreeKey::setKey(const char *newKey) {
    BT_ASSERT(newKey);

    if (newKey) {
        m_key = newKey;
    }
    else {
        positionToRoot();
    }

    return !m_key.popError();
}

QString CSwordTreeKey::getLocalNameUnicode() {
    //return m_module->getTextCodec()->toUnicode(getLocalName());
    //Only UTF-8 and latin1 are legal Sword module encodings
    BT_ASSERT(m_module);
    if (m_module->isUnicode()) {
        return QString::fromUtf8(m_key.getLocalName());
    }
    else {
        return util::cp1252::toUnicode(m_key.getLocalName());
    }
}

void CSwordTreeKey::setModule(const CSwordModuleInfo *newModule) {
    BT_ASSERT(newModule);
    if (m_module == newModule) return;
    BT_ASSERT(newModule->type() == CSwordModuleInfo::GenericBook);
    BT_ASSERT(dynamic_cast<CSwordBookModuleInfo const *>(newModule));

    m_module = newModule;

    const QString oldKey = key();

    m_key.copyFrom(
                *static_cast<CSwordBookModuleInfo const *>(newModule)->tree());

    setKey(oldKey); //try to restore our old key

    //set the key to the root node
    positionToRoot();
    positionToFirstChild();
}
