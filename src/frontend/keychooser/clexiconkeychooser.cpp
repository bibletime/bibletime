/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/keychooser/clexiconkeychooser.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <QHBoxLayout>
#include <QDebug>
#include "backend/config/cbtconfig.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/keys/cswordldkey.h"
#include "frontend/keychooser/bthistory.h"
#include "frontend/keychooser/ckeychooserwidget.h"
#include "frontend/keychooser/cscrollbutton.h"
#include "util/cresmgr.h"


CLexiconKeyChooser::CLexiconKeyChooser(QList<CSwordModuleInfo*> modules, CSwordKey *key, QWidget *parent)
        : CKeyChooser(modules, key, parent),
        m_key(dynamic_cast<CSwordLDKey*>(key)) {
    setModules(modules, false);

    //we use a layout because the key chooser should be resized to full size
    m_layout = new QHBoxLayout(this);
    m_layout->setSpacing(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setDirection(QBoxLayout::LeftToRight);
    m_layout->setSizeConstraint(QLayout::SetNoConstraint);

    m_widget = new CKeyChooserWidget(0, false, this);
    setFocusProxy(m_widget);

    //don't allow a too high width, try to keep as narrow as possible
    //to aid users with smaller screen resolutions
    m_widget->comboBox()->setMaximumWidth(200);

    m_widget->setToolTips(
        tr("Entries of the current work"),
        tr("Next entry"),
        tr("Scroll through the entries of the list. Press the button and move the mouse to increase or decrease the item."),
        tr("Previous entry")
    );

    m_layout->addWidget(m_widget, 0, Qt::AlignLeft);

    connect(m_widget, SIGNAL(changed(int)), SLOT(activated(int)));
    connect(m_widget, SIGNAL(focusOut(int)), SLOT(activated(int)));

    setModules(modules, true);
    setKey(key);
    connect(this, SIGNAL(keyChanged(CSwordKey*)), history(), SLOT(add(CSwordKey*)) );
}

CSwordKey* CLexiconKeyChooser::key() {
    //  qWarning("key");
    return m_key;
}

void CLexiconKeyChooser::setKey(CSwordKey* key) {
    qDebug() << "CLexiconKeyChooser::setKey";

    if (!(m_key = dynamic_cast<CSwordLDKey*>(key))) {
        return;
    }

    QString newKey = m_key->key();
    const int index = m_widget->comboBox()->findText(newKey);
    m_widget->comboBox()->setCurrentIndex(index);

    //   qWarning("setKey end");
    emit keyChanged( m_key);
}

void CLexiconKeyChooser::activated(int index) {
    //  qWarning("activated");
    const QString text = m_widget->comboBox()->itemText(index);

    // To prevent from eternal loop, because activated() is emitted again
    if (m_key && m_key->key() != text) {
        m_key->key(text);
        setKey(m_key);
    }
    //  qWarning("activated end");
}

inline bool my_cmpEntries(const QString& a, const QString& b) {
    return a < b;
}

/** Reimplementation. */
void CLexiconKeyChooser::refreshContent() {
    if (m_modules.count() == 1) {
        m_widget->reset(m_modules.first()->entries(), 0, true);
        //     qWarning("resetted");
    }
    else {
        typedef std::multimap<unsigned int, QStringList*> EntryMap;
        EntryMap entryMap;
        QStringList* entries = 0;
        QListIterator<CSwordLexiconModuleInfo*> mit(m_modules);
        while (mit.hasNext()) {
            entries = mit.next()->entries();
            entryMap.insert( std::make_pair(entries->count(), entries) );
        }

        QStringList goodEntries; //The string list which contains the entries which are available in all modules

        EntryMap::iterator it = entryMap.begin(); //iterator to go thoigh all selected modules
        QStringList refEntries = *(it->second); //copy the items for the first time
        QStringList* cmpEntries = ( ++it )->second; //list for comparision, starts with the second module in the map

        while (it != entryMap.end()) {
            std::set_intersection(
                refEntries.begin(), --(refEntries.end()), //--end() is the last valid entry
                cmpEntries->begin(), --(cmpEntries->end()),
                std::back_inserter(goodEntries), //append valid entries to the end of goodEntries
                my_cmpEntries  //ci_cmpEntries is the comparision function
            );

            cmpEntries = ( ++it )->second; //this is a pointer to the string list of a new module

            /*
            * use the good entries for next comparision,
            * because the final list can only have the entries of goodEntries as maxiumum
            */
            refEntries = goodEntries;
        };

        m_widget->reset(goodEntries, 0, true); //write down the entries
    } //end of ELSE

}

/** No descriptions */
void CLexiconKeyChooser::adjustFont() {

}

/** Sets the module and refreshes the combo boxes */
void CLexiconKeyChooser::setModules( const QList<CSwordModuleInfo*>& modules, const bool refresh ) {

    while (!m_modules.isEmpty())
        m_modules.takeFirst(); // not deleting the pointer

    QList<CSwordModuleInfo*>::const_iterator end_it = modules.end();
    for (QList<CSwordModuleInfo*>::const_iterator it(modules.begin()); it != end_it; ++it) {
        CSwordLexiconModuleInfo* lexicon = dynamic_cast<CSwordLexiconModuleInfo*>(*it);
        if (lexicon) {
            m_modules.append(lexicon);
        }
    }

    if (refresh) {
        refreshContent();
        //   adjustFont();
    }
}

/** No descriptions */
void CLexiconKeyChooser::updateKey(CSwordKey*) {}

void CLexiconKeyChooser::setKey(QString& newKey) {
    m_key->key(newKey);
    setKey(m_key);
}
