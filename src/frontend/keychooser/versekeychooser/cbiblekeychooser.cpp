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

#include "cbiblekeychooser.h"

#include <QDebug>
#include <QHBoxLayout>
#include "../../../backend/drivers/cswordbiblemoduleinfo.h"
#include "../../../backend/drivers/cswordmoduleinfo.h"
#include "../../../backend/keys/cswordversekey.h"
#include "../../../util/btassert.h"
#include "../../../util/btconnect.h"
#include "btbiblekeywidget.h"


CBibleKeyChooser::CBibleKeyChooser(const BtConstModuleList & modules,
                                   CSwordKey * key,
                                   QWidget * parent)
    : CKeyChooser(parent)
    , m_key(dynamic_cast<CSwordVerseKey *>(key))
{
    setModules(modules, false);
    BT_ASSERT(!m_modules.empty());
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setDirection( QBoxLayout::LeftToRight );

    m_widget = new BtBibleKeyWidget(m_modules.first(), m_key, this);
    setFocusProxy(m_widget);
    layout->addWidget(m_widget);

    BT_CONNECT(m_widget, &BtBibleKeyWidget::changed,
               [this](CSwordVerseKey * key) {
                   BT_ASSERT(m_key);
                   BT_ASSERT(key);

                   if (!updatesEnabled())
                       return;

                   setUpdatesEnabled(false);
                   m_key = key;
                   Q_EMIT keyChanged(m_key);

                   setUpdatesEnabled(true);
               });

    setKey(m_key); //set the key without changing it, setKey(key()) would change it
}

CSwordKey* CBibleKeyChooser::key() {
    return m_key;
}

void CBibleKeyChooser::setKey(CSwordKey* key) {
    BT_ASSERT(dynamic_cast<CSwordVerseKey *>(key));
    if (dynamic_cast<CSwordVerseKey*>(key) == nullptr) return;

    m_key = dynamic_cast<CSwordVerseKey*>(key);
    m_widget->setKey(m_key);
    Q_EMIT keyChanged(m_key);
}

void CBibleKeyChooser::setModules(const BtConstModuleList &modules,
                                  bool refresh)
{
    using CSBMI = CSwordBibleModuleInfo;

    m_modules.clear();

    for (auto const * const mod : modules)
        if (mod->type() == CSwordModuleInfo::Bible
            || mod->type() == CSwordModuleInfo::Commentary)
            if (CSBMI const * const bible = dynamic_cast<CSBMI const *>(mod))
                m_modules.append(bible);

    // First time this is called we havnt set up m_widget.
    if (m_widget)
        m_widget->setModule(m_modules.first());
    if (refresh) refreshContent();
}

void CBibleKeyChooser::refreshContent() {
    setKey(m_key);
}

void CBibleKeyChooser::updateKey(CSwordKey* /*key*/) {
    m_widget->updateText();
}
