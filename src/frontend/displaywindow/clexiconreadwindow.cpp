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

#include "clexiconreadwindow.h"

#include <QAction>
#include <QString>
#include "../../backend/keys/cswordkey.h"
#include "../../backend/keys/cswordldkey.h"
#include "../../util/cresmgr.h"
#include "../keychooser/ckeychooser.h"
#include "btactioncollection.h"


CLexiconReadWindow::ActionCollection::ActionCollection(QObject * const parent)
    : CDisplayWindow::ActionCollection(parent)
{
    auto * actn = new QAction(tr("Next entry"), this);
    actn->setShortcut(CResMgr::displaywindows::lexiconWindow::nextEntry::accel);
    addAction(QStringLiteral("nextEntry"), actn);

    actn = new QAction(tr("Previous entry"), this);
    actn->setShortcut(CResMgr::displaywindows::lexiconWindow::previousEntry::accel);
    addAction(QStringLiteral("previousEntry"), actn);
}

CLexiconReadWindow::CLexiconReadWindow(
        QList<CSwordModuleInfo *> const & modules,
        QString const & key,
        CMDIArea * parent)
    : CDisplayWindow(modules, key, true, new ActionCollection(), parent)
{ init(); }

void CLexiconReadWindow::initActions() {
    initAddAction(QStringLiteral("nextEntry"),
                  this,
                  &CLexiconReadWindow::nextEntry);
    initAddAction(QStringLiteral("previousEntry"),
                  this,
                  &CLexiconReadWindow::previousEntry);
    m_actionCollection->readShortcuts(QStringLiteral("Lexicon shortcuts"));
}

void CLexiconReadWindow::reload() {
    CDisplayWindow::reload();
    actionCollection()->readShortcuts(QStringLiteral("Lexicon shortcuts"));
}

void CLexiconReadWindow::nextEntry()
{ keyChooser()->setKey(ldKey()->NextEntry()); }

void CLexiconReadWindow::previousEntry()
{ keyChooser()->setKey(ldKey()->PreviousEntry()); }

CSwordLDKey * CLexiconReadWindow::ldKey()
{ return dynamic_cast<CSwordLDKey*>(m_swordKey.get()); }
