/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/keychooser/ckeychooser.h"

#include <QAction>
#include "backend/drivers/cswordbiblemoduleinfo.h"
#include "backend/drivers/cswordcommentarymoduleinfo.h"
#include "backend/drivers/cswordlexiconmoduleinfo.h"
#include "backend/drivers/cswordmoduleinfo.h"
#include "backend/keys/cswordkey.h"
#include "frontend/keychooser/bthistory.h"
#include "frontend/keychooser/cbookkeychooser.h"
#include "frontend/keychooser/clexiconkeychooser.h"
#include "frontend/keychooser/versekeychooser/cbiblekeychooser.h"


CKeyChooser::CKeyChooser(const QList<const CSwordModuleInfo *> &,
                         BTHistory * historyPtr,
                         QWidget * parent)
    : QWidget(parent)
    , m_history(historyPtr)
{
    bool ok = QObject::connect(history(), SIGNAL(historyMoved(QString)),
                               this,      SLOT(setKey(const QString &)));
    Q_ASSERT(ok);
}

CKeyChooser * CKeyChooser::createInstance(const QList<const CSwordModuleInfo *> & modules,
                                          BTHistory * historyPtr,
                                          CSwordKey * key,
                                          QWidget * parent)
{
    Q_ASSERT(!modules.empty());
    Q_ASSERT(modules.first()->type() == CSwordModuleInfo::Commentary
             || modules.first()->type() == CSwordModuleInfo::Bible
             || modules.first()->type() == CSwordModuleInfo::Lexicon
             || modules.first()->type() == CSwordModuleInfo::GenericBook);

    switch (modules.first()->type()) {

        case CSwordModuleInfo::Commentary:
            /* Fall thru - Bibles and commentaries use the same key chooser */

        case CSwordModuleInfo::Bible:
            return new CBibleKeyChooser(modules, historyPtr, key, parent);

        case CSwordModuleInfo::Lexicon:
            return new CLexiconKeyChooser(modules, historyPtr, key, parent);

        case CSwordModuleInfo::GenericBook:
            return new CBookKeyChooser(modules, historyPtr, key, parent);

        default:
            abort();

    }
}
