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

#include "clexiconkeychooser.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <QBoxLayout>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLayout>
#include <QStringList>
#include <Qt>
#include <utility>
#include "../../backend/drivers/btmodulelist.h"
#include "../../backend/drivers/cswordlexiconmoduleinfo.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/keys/cswordkey.h"
#include "../../backend/keys/cswordldkey.h"
#include "../../util/btconnect.h"
#include "ckeychooserwidget.h"


CLexiconKeyChooser::CLexiconKeyChooser(const BtConstModuleList & modules,
                                       CSwordKey * key,
                                       QWidget * parent)
    : CKeyChooser(parent)
    , m_key(dynamic_cast<CSwordLDKey *>(key))
{
    setModules(modules, false);

    //we use a layout because the key chooser should be resized to full size
    auto * const l = new QHBoxLayout(this);
    l->setSpacing(0);
    l->setContentsMargins(0, 0, 0, 0);
    l->setDirection(QBoxLayout::LeftToRight);
    l->setSizeConstraint(QLayout::SetNoConstraint);

    m_widget = new CKeyChooserWidget(0, this);
    setFocusProxy(m_widget);

    //don't allow a too high width, try to keep as narrow as possible
    //to aid users with smaller screen resolutions
    m_widget->comboBox().setMaximumWidth(200);

    m_widget->setToolTips(
        tr("Entries of the current work"),
        tr("Next entry"),
        tr("Scroll through the entries of the list. Press the button and move the mouse to increase or decrease the item."),
        tr("Previous entry")
    );

    l->addWidget(m_widget, 0, Qt::AlignLeft);

    auto const activatedSlot =
            [this](int index) {
                if (m_key) {
                    auto text(m_widget->comboBox().itemText(index));
                    /* Check to prevent from eternal loop, because activated()
                       is emitted again: */
                    if (m_key->key() != text) {
                        m_key->setKey(std::move(text));
                        setKey(m_key);
                    }
                }
            };
    BT_CONNECT(m_widget, &CKeyChooserWidget::changed, activatedSlot);
    BT_CONNECT(m_widget, &CKeyChooserWidget::focusOut, activatedSlot);

    setModules(modules, true);
    setKey(key);
}

CSwordKey* CLexiconKeyChooser::key() {
    //  qWarning("key");
    return m_key;
}

/** Update key display without emiting a signal */
void CLexiconKeyChooser::updateKey(CSwordKey* key) {
    if (!(m_key = dynamic_cast<CSwordLDKey*>(key))) {
        return;
    }

    QString newKey = m_key->key();
    const int index = m_widget->comboBox().findText(newKey);
    m_widget->comboBox().setCurrentIndex(index);
}

void CLexiconKeyChooser::setKey(CSwordKey* key) {
    if (!(m_key = dynamic_cast<CSwordLDKey*>(key))) {
        return;
    }

    updateKey(key);

    //   qWarning("setKey end");
    Q_EMIT keyChanged( m_key);
}

/** Reimplementation. */
void CLexiconKeyChooser::refreshContent() {
    if (m_modules.count() == 1) {
        m_widget->reset(&m_modules.first()->entries(), 0, true);
        //     qWarning("resetted");
    }
    else {
        std::multimap<unsigned int, QStringList const *> entryMap;
        for (auto const * const modulePtr : m_modules) {
            auto const & entries = modulePtr->entries();
            entryMap.emplace(entries.count(), &entries);
        }

        QStringList goodEntries; //The string list which contains the entries which are available in all modules

        auto it(entryMap.begin()); // iterator to go though all selected modules
        QStringList refEntries = *(it->second); //copy the items for the first time
        const QStringList *cmpEntries = (++it)->second; //list for comparision, starts with the second module in the map

        // Testing for refEntries being empty is not needed for the set union
        // of all keys, but is a good idea since it is being updated in the
        // loop.  It is necessary for set intersection and prevents a crash.
        while (it != entryMap.end() && (refEntries.begin() != refEntries.end())) {
            std::set_union(
                refEntries.begin(), --(refEntries.end()), //--end() is the last valid entry
                cmpEntries->begin(), --(cmpEntries->end()),
                std::back_inserter(goodEntries) //append valid entries to the end of goodEntries
            );

            cmpEntries = ( ++it )->second; //this is a pointer to the string list of a new module

            /*
            * use the good entries for next comparision,
            * because the final list can only have the entries of goodEntries as maxiumum
            */
            refEntries = goodEntries;
        }

        m_widget->reset(goodEntries, 0, true); //write down the entries
    } //end of ELSE

}

void CLexiconKeyChooser::setModules(const BtConstModuleList &modules,
                                    bool refresh)
{
    using CSLMI = CSwordLexiconModuleInfo;

    while (!m_modules.isEmpty())
        m_modules.takeFirst(); // not deleting the pointer

    for (auto const * const m : modules)
        if (CSLMI const * const lexicon = dynamic_cast<CSLMI const *>(m))
            m_modules.append(lexicon);

    if (refresh) {
        refreshContent();
    }
}
