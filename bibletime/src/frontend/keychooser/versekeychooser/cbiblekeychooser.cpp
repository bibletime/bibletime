/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/keychooser/versekeychooser/cbiblekeychooser.h"

#include <QDebug>
#include <QHBoxLayout>
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordversekey.h"
#include "frontend/keychooser/bthistory.h"
#include "frontend/keychooser/cscrollbutton.h"
#include "frontend/keychooser/versekeychooser/btbiblekeywidget.h"
#include "util/cresmgr.h"


CBibleKeyChooser::CBibleKeyChooser(QList<CSwordModuleInfo*> modules, 
                                   BTHistory* historyPtr, 
                                   CSwordKey *key, 
                                   QWidget *parent)
        : CKeyChooser(modules, historyPtr, key, parent),
        m_key(dynamic_cast<CSwordVerseKey*>(key)) {
    w_ref = 0;
    setModules(modules, false);
    if (!m_modules.count()) {
        qWarning() << "CBibleKeyChooser: module is not a Bible or commentary!";
        m_key = 0;
        return;
    }
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setDirection( QBoxLayout::LeftToRight );

    w_ref = new BtBibleKeyWidget(dynamic_cast<CSwordBibleModuleInfo*>(m_modules.first()), m_key, this);
    setFocusProxy(w_ref);
    layout->addWidget(w_ref);

    bool ok = connect(w_ref, SIGNAL(beforeChange(CSwordVerseKey *)), SLOT(beforeRefChange(CSwordVerseKey *)));
    connect(w_ref, SIGNAL(changed(CSwordVerseKey *)), SLOT(refChanged(CSwordVerseKey *)));
    Q_ASSERT(ok);

    setKey(m_key); //set the key without changing it, setKey(key()) would change it

    ok = connect(this, SIGNAL(keyChanged(CSwordKey*)), history(), SLOT(add(CSwordKey*)) );
    Q_ASSERT(ok);
}

CSwordKey* CBibleKeyChooser::key() {
    return m_key;
}

void CBibleKeyChooser::setKey(CSwordKey* key) {
    Q_ASSERT(dynamic_cast<CSwordVerseKey*>(key));
    if (dynamic_cast<CSwordVerseKey*>(key) == 0) return;

    emit (beforeKeyChange(m_key->key())); //required to make direct setKey calls work from the outside
    m_key = dynamic_cast<CSwordVerseKey*>(key);
    w_ref->setKey(m_key);
    emit keyChanged(m_key);
}

void CBibleKeyChooser::beforeRefChange(CSwordVerseKey* key) {
    Q_UNUSED(key);

    Q_ASSERT(m_key);

    if (!updatesEnabled())
        return;

    if (m_key)
        emit beforeKeyChange(m_key->key());

}

void CBibleKeyChooser::refChanged(CSwordVerseKey* key) {
    Q_ASSERT(m_key);
    Q_ASSERT(key);

    if (!updatesEnabled())
        return;

    setUpdatesEnabled(false);
    m_key = key;
    emit keyChanged(m_key);

    setUpdatesEnabled(true);
}

void CBibleKeyChooser::setModules(const QList<CSwordModuleInfo*>& modules, const bool refresh) {
    m_modules.clear();

    foreach (CSwordModuleInfo* mod, modules) {
        if (mod->type() == CSwordModuleInfo::Bible || mod->type() == CSwordModuleInfo::Commentary) {
            if (CSwordBibleModuleInfo* bible = dynamic_cast<CSwordBibleModuleInfo*>(mod)) m_modules.append(bible);
        }
    }

    // First time this is called we havnt set up w_ref.
    if (w_ref) w_ref->setModule(dynamic_cast<CSwordBibleModuleInfo*>(m_modules.first()));
    if (refresh) refreshContent();
}

void CBibleKeyChooser::refreshContent() {
    setKey(m_key);
}

void CBibleKeyChooser::updateKey(CSwordKey* /*key*/) {
    w_ref->updateText();
}

void CBibleKeyChooser::adjustFont() {}

void CBibleKeyChooser::setKey(QString& newKey) {
    m_key->key(newKey);
    setKey(m_key);
}
