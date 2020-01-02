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

#include "ckeychooser.h"

#include <QAction>
#include "../../backend/drivers/cswordbiblemoduleinfo.h"
#include "../../backend/drivers/cswordcommentarymoduleinfo.h"
#include "../../backend/drivers/cswordlexiconmoduleinfo.h"
#include "../../backend/drivers/cswordmoduleinfo.h"
#include "../../backend/keys/cswordkey.h"
#include "../../util/btassert.h"
#include "../../util/btconnect.h"
#include "bthistory.h"
#include "cbookkeychooser.h"
#include "clexiconkeychooser.h"
#include "versekeychooser/cbiblekeychooser.h"


CKeyChooser::CKeyChooser(const BtConstModuleList &,
                         BTHistory * historyPtr,
                         QWidget * parent)
    : QWidget(parent)
    , m_history(historyPtr)
{
    BT_CONNECT(history(), SIGNAL(historyMoved(QString)),
               this,      SLOT(setKey(QString const &)));
}

CKeyChooser * CKeyChooser::createInstance(const BtConstModuleList & modules,
                                          BTHistory * historyPtr,
                                          CSwordKey * key,
                                          QWidget * parent)
{
    BT_ASSERT(!modules.empty());
    BT_ASSERT(modules.first()->type() == CSwordModuleInfo::Commentary
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
