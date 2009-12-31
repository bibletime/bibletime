/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "backend/keys/cswordtreekey.h"

#include <QDebug>
#include <QTextCodec>
#include "backend/drivers/cswordbookmoduleinfo.h"


CSwordTreeKey::CSwordTreeKey( const CSwordTreeKey& k ) : CSwordKey(k), TreeKeyIdx(k) {}

CSwordTreeKey::CSwordTreeKey( const TreeKeyIdx *k, CSwordModuleInfo* module ) : CSwordKey(module), TreeKeyIdx(*k) {}

CSwordTreeKey* CSwordTreeKey::copy() const {
    return new CSwordTreeKey(*this);
}

/** Sets the key of this instance */
QString CSwordTreeKey::key() const {
    //return getTextUnicode();
    Q_ASSERT(m_module);
    if (m_module->isUnicode()) {
        return QString::fromUtf8(getText());
    }
    else {
        return cp1252Codec()->toUnicode(getText());
    }
}

const char * CSwordTreeKey::rawKey() const {
    return getText();
}

bool CSwordTreeKey::key( const QString& newKey ) {
    //return key( newKey.toLocal8Bit().constData() );
    //return key(m_module->getTextCodec()->fromUnicode(newKey).constData());
    Q_ASSERT(m_module);
    if (m_module->isUnicode()) {
        return key(newKey.toUtf8().constData());
    }
    else {
        return key((const char*)cp1252Codec()->fromUnicode(newKey));
    }
}

bool CSwordTreeKey::key( const char* newKey ) {
    Q_ASSERT(newKey);

    if (newKey) {
        TreeKeyIdx::operator = (newKey);
    }
    else {
        root();
    }

    return !Error();
}

QString CSwordTreeKey::getLocalNameUnicode() {
    //return m_module->getTextCodec()->toUnicode(getLocalName());
    //Only UTF-8 and latin1 are legal Sword module encodings
    Q_ASSERT(m_module);
    if (m_module->isUnicode()) {
        return QString::fromUtf8(getLocalName());
    }
    else {
        return cp1252Codec()->toUnicode(getLocalName());
    }
}

CSwordModuleInfo* CSwordTreeKey::module( CSwordModuleInfo* const newModule ) {
    if (newModule && (newModule != m_module) && (newModule->type() == CSwordModuleInfo::GenericBook) ) {
        m_module = newModule;

        const QString oldKey = key();

        CSwordBookModuleInfo* newBook = dynamic_cast<CSwordBookModuleInfo*>(newModule);
        copyFrom( *(newBook->tree()) );

        key(oldKey); //try to restore our old key

        //set the key to the root node
        root();
        firstChild();
    }

    return m_module;
}
