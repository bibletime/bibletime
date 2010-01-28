/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/clexiconreadwindow.h"

#include <QAction>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QDebug>

#include "backend/config/cbtconfig.h"
#include "backend/keys/cswordldkey.h"
#include "backend/keys/cswordkey.h"
#include "frontend/cexportmanager.h"
#include "frontend/display/bthtmlreaddisplay.h"
#include "frontend/display/cdisplay.h"
#include "frontend/display/creaddisplay.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/bttoolbarpopupaction.h"
#include "frontend/displaywindow/btdisplaysettingsbutton.h"
#include "frontend/displaywindow/bttextwindowheader.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/keychooser/bthistory.h"
#include "frontend/keychooser/ckeychooser.h"
#include "util/directory.h"
#include "util/cresmgr.h"
#include "util/tool.h"


CLexiconReadWindow::CLexiconReadWindow(QList<CSwordModuleInfo*> moduleList, CMDIArea* parent)
        : CReadWindow(moduleList, parent) {
    qDebug() << "CLexiconReadWindow::CLexiconReadWindow";
    moduleList.first();
    setKey( CSwordKey::createInstance(moduleList.first()) );
}

CLexiconReadWindow::~CLexiconReadWindow() {
}

void CLexiconReadWindow::insertKeyboardActions( BtActionCollection* const a ) {
    qDebug() << "CLexiconReadWindow::insertKeyboardActions";
    QAction* qaction;
    qaction = new QAction( tr("Next entry"), a);
    qaction->setShortcut(CResMgr::displaywindows::lexiconWindow::nextEntry::accel);
    a->addAction("nextEntry", qaction);

    qaction = new QAction( tr("Previous entry"), a);
    qaction->setShortcut( CResMgr::displaywindows::lexiconWindow::previousEntry::accel);
    a->addAction("previousEntry", qaction);

    qaction = new QAction(tr("Copy reference only"), a);
    a->addAction("copyReferenceOnly", qaction);

    qaction = new QAction(tr("Save entry as HTML"), a);
    a->addAction("saveHtml", qaction);

    qaction = new QAction(tr("Print reference only"), a);
    a->addAction("printReferenceOnly", qaction);

    qaction = new QAction(tr("Entry with text"), a);
    a->addAction("copyEntryWithText", qaction);

    qaction = new QAction(tr("Entry as plain text"), a);
    a->addAction("saveEntryAsPlain", qaction);

    qaction = new QAction(tr("Entry with text"), a);
    a->addAction("printEntryWithText", qaction);

    qaction = new QAction( /* QIcon(CResMgr::displaywindows::general::findStrongs::icon), */ tr("Strong's Search"), a);
    qaction->setShortcut(CResMgr::displaywindows::general::findStrongs::accel);
    a->addAction(CResMgr::displaywindows::general::findStrongs::actionName, qaction);
}

void CLexiconReadWindow::initActions() {
    qDebug() << "CLexiconReadWindow::initActions";

    BtActionCollection* ac = actionCollection();
    CReadWindow::initActions();
    CLexiconReadWindow::insertKeyboardActions(ac);

    m_actions.backInHistory = dynamic_cast<BtToolBarPopupAction*>(
                                  ac->action(CResMgr::displaywindows::general::backInHistory::actionName) );
    Q_ASSERT(m_actions.backInHistory);
    addAction(m_actions.backInHistory);

    m_actions.forwardInHistory = dynamic_cast<BtToolBarPopupAction*>(
                                     ac->action(CResMgr::displaywindows::general::forwardInHistory::actionName) );
    Q_ASSERT(m_actions.forwardInHistory);
    addAction(m_actions.forwardInHistory);

    QAction* qaction;

    qaction = ac->action("nextEntry");
    QObject::connect(qaction, SIGNAL(triggered()), this, SLOT( nextEntry() ) );
    addAction(qaction);

    qaction = ac->action("previousEntry");
    QObject::connect(qaction, SIGNAL(triggered()), this, SLOT( previousEntry() ) );
    addAction(qaction);

    m_actions.selectAll = ac->action("selectAll");
    Q_ASSERT(m_actions.selectAll);

    m_actions.findText = ac->action("findText");
    Q_ASSERT(m_actions.findText);

    m_actions.findStrongs = ac->action(CResMgr::displaywindows::general::findStrongs::actionName);
    QObject::connect(m_actions.findStrongs, SIGNAL(triggered()), this, SLOT(openSearchStrongsDialog()) );
    addAction(m_actions.findStrongs);

    m_actions.copy.reference = ac->action("copyReferenceOnly");
    QObject::connect(m_actions.copy.reference, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(copyAnchorOnly()) );
    addAction(m_actions.copy.reference);

    m_actions.copy.entry = ac->action("copyEntryWithText");
    QObject::connect(m_actions.copy.entry, SIGNAL(triggered()), displayWidget()->connectionsProxy(), SLOT(copyAll()) );
    addAction(m_actions.copy.entry);

    Q_ASSERT(ac->action("copySelectedText"));
    m_actions.copy.selectedText = ac->action("copySelectedText");

    m_actions.save.entryAsPlain = new QAction(tr("Entry as plain text"), ac );
    QObject::connect(m_actions.save.entryAsPlain, SIGNAL(triggered()), this, SLOT(saveAsPlain()) );
    addAction(m_actions.save.entryAsPlain);

    m_actions.save.entryAsHTML = ac->action("saveHtml");
    QObject::connect(m_actions.save.entryAsHTML, SIGNAL(triggered()), this, SLOT(saveAsHTML()));
    addAction(m_actions.save.entryAsHTML);

    m_actions.print.reference = ac->action("printReferenceOnly");
    QObject::connect(m_actions.print.reference, SIGNAL(triggered()), this, SLOT(printAnchorWithText()));
    addAction(m_actions.print.reference);

    m_actions.print.entry = ac->action("printEntryWithText");
    QObject::connect(m_actions.print.entry, SIGNAL(triggered()), this, SLOT(printAll()));
    addAction(m_actions.print.entry);

    // init with the user defined settings
    qDebug() << "call CBTConfig::setupAccelSettings(CBTConfig::lexiconWindow, ac); and end CLexiconReadWindow::initActions";
    CBTConfig::setupAccelSettings(CBTConfig::lexiconWindow, ac);
}

/** No descriptions */
void CLexiconReadWindow::initConnections() {
    qDebug() << "CLexiconReadWindow::initConnections";
    Q_ASSERT(keyChooser());

    connect(keyChooser(), SIGNAL(keyChanged(CSwordKey*)), this, SLOT(lookupSwordKey(CSwordKey*)));
    connect(keyChooser()->history(), SIGNAL(historyChanged(bool, bool)), this, SLOT(slotUpdateHistoryButtons(bool, bool)));

    //connect the history actions to the right slots
    bool ok = connect(
                  m_actions.backInHistory->popupMenu(), SIGNAL(aboutToShow()),
                  this, SLOT(slotFillBackHistory())
              );
    Q_ASSERT(ok);
    ok = connect(
             m_actions.backInHistory->popupMenu(), SIGNAL(triggered(QAction*)),
             keyChooser()->history(), SLOT(move(QAction*))
         );
    Q_ASSERT(ok);
    ok = connect(
             m_actions.forwardInHistory->popupMenu(), SIGNAL(aboutToShow()),
             this, SLOT(slotFillForwardHistory())
         );
    Q_ASSERT(ok);
    ok = connect(
             m_actions.forwardInHistory->popupMenu(), SIGNAL(triggered(QAction*)),
             keyChooser()->history(), SLOT(move(QAction*))
         );
    Q_ASSERT(ok);

}

void CLexiconReadWindow::initView() {
    qDebug() << "CLexiconReadWindow::initView";

    // Create display widget for this window
    setDisplayWidget( CDisplay::createReadInstance(this) );
    setCentralWidget( displayWidget()->view() );
    setWindowIcon(util::tool::getIconForModule(modules().first()));

    // Create the Navigation toolbar
    setMainToolBar( new QToolBar(this) );
    addToolBar(mainToolBar());

    // Create keychooser
    setKeyChooser( CKeyChooser::createInstance(modules(), history(), key(), mainToolBar()) );

    // Create the Works toolbar
    setModuleChooserBar( new BtModuleChooserBar(getModuleList(), modules().first()->type(), this) );
    addToolBar(moduleChooserBar());

    // Create the Tools toolbar
    setButtonsToolBar( new QToolBar(this) );
    addToolBar(buttonsToolBar());

    // Create the Text Header toolbar
    addToolBarBreak();
    setHeaderBar(new QToolBar(this));
    addToolBar(headerBar());
}

void CLexiconReadWindow::initToolbars() {
    //Navigation toolbar
    Q_ASSERT(m_actions.backInHistory);
    mainToolBar()->addWidget(keyChooser());
    mainToolBar()->addAction(m_actions.backInHistory); //1st button
    mainToolBar()->addAction(m_actions.forwardInHistory); //2nd button

    //Tools toolbar
    QAction* action = qobject_cast<QAction*>(actionCollection()->action(
                          CResMgr::displaywindows::general::search::actionName));
    Q_ASSERT( action );
    if (action) {
        buttonsToolBar()->addAction(action);
    }
    setDisplaySettingsButton(new BtDisplaySettingsButton(buttonsToolBar()));
    /// \todo find the right place for the button
    buttonsToolBar()->addWidget(displaySettingsButton());

    // Text Header toolbar
    BtTextWindowHeader* h = new BtTextWindowHeader(this,  modules().first()->type(), getModuleList());
    headerBar()->addWidget(h);
}

void CLexiconReadWindow::setupPopupMenu() {
    popup()->setTitle(tr("Lexicon window"));
    popup()->setIcon(util::tool::getIconForModule(modules().first()));
    popup()->addAction(m_actions.findText);
    popup()->addAction(m_actions.findStrongs);
    popup()->addAction(m_actions.selectAll);
    popup()->addSeparator();

    m_actions.copyMenu = new QMenu(tr("Copy..."), popup());

    m_actions.copyMenu->addAction(m_actions.copy.reference);
    m_actions.copyMenu->addAction(m_actions.copy.entry);
    m_actions.copyMenu->addSeparator();
    m_actions.copyMenu->addAction(m_actions.copy.selectedText);
    popup()->addMenu(m_actions.copyMenu);

    m_actions.saveMenu = new QMenu(
        tr("Save..."),
        popup()
    );
    m_actions.saveMenu->addAction(m_actions.save.entryAsPlain);
    m_actions.saveMenu->addAction(m_actions.save.entryAsHTML);

    // Save raw HTML action for debugging purposes
    if (qApp->property("--debug").toBool()) {
        QAction* debugAction = new QAction("Raw HTML", this);
        QObject::connect(debugAction, SIGNAL(triggered()), this, SLOT(saveRawHTML()));
        m_actions.saveMenu->addAction(debugAction);
    } // end of Save Raw HTML

    popup()->addMenu(m_actions.saveMenu);

    m_actions.printMenu = new QMenu(
        tr("Print..."),
        popup()
    );
    m_actions.printMenu->addAction(m_actions.print.reference);
    m_actions.printMenu->addAction(m_actions.print.entry);
    popup()->addMenu(m_actions.printMenu);
}

/** Reimplemented. */
void CLexiconReadWindow::updatePopupMenu() {
    //enable the action depending on the supported module features

    m_actions.findStrongs->setEnabled( displayWidget()->getCurrentNodeInfo()[CDisplay::Lemma] != QString::null );

    m_actions.copy.reference->setEnabled( ((CReadDisplay*)displayWidget())->hasActiveAnchor() );
    m_actions.copy.selectedText->setEnabled( displayWidget()->hasSelection() );

    m_actions.print.reference->setEnabled( ((CReadDisplay*)displayWidget())->hasActiveAnchor() );
}

void CLexiconReadWindow::reload(CSwordBackend::SetupChangedReason reason) {
    CReadWindow::reload(reason);

    CBTConfig::setupAccelSettings(CBTConfig::lexiconWindow, actionCollection());
}

/** No descriptions */
void CLexiconReadWindow::nextEntry() {
    keyChooser()->setKey(ldKey()->NextEntry());
}

/** No descriptions */
void CLexiconReadWindow::previousEntry() {
    keyChooser()->setKey(ldKey()->PreviousEntry());
}

/** Reimplementation to return the right key. */
CSwordLDKey* CLexiconReadWindow::ldKey() {
    return dynamic_cast<CSwordLDKey*>(CDisplayWindow::key());
}

/** This function saves the entry as html using the CExportMgr class. */
void CLexiconReadWindow::saveAsHTML() {
    CExportManager mgr(tr("Saving entry ..."), true, tr("Saving"), filterOptions(), displayOptions());
    mgr.saveKey(key(), CExportManager::HTML, true);
}

/** Saving the raw HTML for debugging purposes */
void CLexiconReadWindow::saveRawHTML() {
    //qDebug() << "CLexiconReadWindow::saveRawHTML";
    QString savefilename = QFileDialog::getSaveFileName();
    if (savefilename.isEmpty()) return;
    QFile file(savefilename);
    BtHtmlReadDisplay* disp = dynamic_cast<BtHtmlReadDisplay*>(displayWidget());
    if (disp) {
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "could not open file";
            return;
        }
        QString source = disp->getCurrentSource();
        file.write(source.toUtf8());
        //qDebug() << "wrote" << bytes << "bytes";
        file.close();
        file.flush();
    }
    else {
        qDebug() << "No htmlreaddisplay widget!";
    }

}

/** This function saves the entry as html using the CExportMgr class. */
void CLexiconReadWindow::saveAsPlain() {
    CExportManager mgr(tr("Saving entry ..."), true, tr("Saving"), filterOptions(), displayOptions());
    mgr.saveKey(key(), CExportManager::Text, true);
}

void CLexiconReadWindow::slotFillBackHistory() {
    qDebug() << "CLexiconReadWindow::slotFillBackHistory";

    QMenu* menu = m_actions.backInHistory->popupMenu();
    menu->clear();

    /// \todo take the history list and fill the menu
    QListIterator<QAction*> it(keyChooser()->history()->getBackList());
    while (it.hasNext()) {
        menu->addAction(it.next());
    }
}

void CLexiconReadWindow::slotFillForwardHistory() {
    qDebug() << "CLexiconReadWindow::slotFillForwardHistory";

    QMenu* menu = m_actions.forwardInHistory->popupMenu();
    menu->clear();
    /// \todo take the history list and fill the menu using addAction
    QListIterator<QAction*> it(keyChooser()->history()->getFwList());
    while (it.hasNext()) {
        menu->addAction(it.next());
    }
}


void CLexiconReadWindow::slotUpdateHistoryButtons(bool backEnabled, bool fwEnabled) {
    qDebug() << "CLexiconReadWindow::slotUpdateHistoryButtons";
    Q_ASSERT(m_actions.backInHistory);
    Q_ASSERT(keyChooser());

    m_actions.backInHistory->setEnabled( backEnabled );
    m_actions.forwardInHistory->setEnabled( fwEnabled );
}
