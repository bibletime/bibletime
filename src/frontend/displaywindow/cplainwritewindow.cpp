/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2011 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/cplainwritewindow.h"

#include <QAction>
#include <QDebug>
#include <QRegExp>
#include <QToolBar>
#include "bibletime.h"
#include "backend/keys/cswordkey.h"
#include "frontend/display/cwritedisplay.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/profile/cprofilewindow.h"
#include "util/cresmgr.h"
#include "util/directory.h"
#include "util/dialogutil.h"


using namespace Profile;

CPlainWriteWindow::CPlainWriteWindow(QList<CSwordModuleInfo*> moduleList, CMDIArea* parent) :
        CWriteWindow(moduleList, parent) {
    setKey( CSwordKey::createInstance(moduleList.first()) );
}


CPlainWriteWindow::~CPlainWriteWindow() {}

/** Initialize the state of this widget. */
void CPlainWriteWindow::initView() {
    //  qWarning("CPlainWriteWindow::initView()");
    setDisplayWidget( CDisplay::createWriteInstance(this) );
    setCentralWidget( displayWidget()->view() );

    // Create Navigation toolbar
    setMainToolBar( new QToolBar(this) );
    addToolBar(mainToolBar());

    // Create the Tools toolbar
    setButtonsToolBar( new QToolBar(this) );
    addToolBar(buttonsToolBar());
}

void CPlainWriteWindow::initToolbars() {
    namespace DU = util::directory;

    // Navigation toolbar
    setKeyChooser( CKeyChooser::createInstance(modules(),
        history(), key(), mainToolBar()) );
    mainToolBar()->addWidget(keyChooser());

    // Tools toolbar
    QAction* action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    buttonsToolBar()->addAction(action);
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::saveText::actionName);
    buttonsToolBar()->addAction(action);
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::deleteEntry::actionName);
    buttonsToolBar()->addAction(action);
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::restoreText::actionName);
    buttonsToolBar()->addAction(action);
}

void CPlainWriteWindow::setupMainWindowToolBars() {
    // Navigation toolbar
    CKeyChooser* keyChooser = CKeyChooser::createInstance(modules(), history(), key(), btMainWindow()->navToolBar() );
    btMainWindow()->navToolBar()->addWidget(keyChooser);
    bool ok = connect(keyChooser, SIGNAL(keyChanged(CSwordKey*)), this, SLOT(lookupSwordKey(CSwordKey*)));
    Q_ASSERT(ok);
    ok = connect(keyChooser, SIGNAL(beforeKeyChange(const QString&)), this, SLOT(beforeKeyChange(const QString&)));
    Q_ASSERT(ok);

    // Tools toolbar
    QAction* action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    btMainWindow()->toolsToolBar()->addAction(action);
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::saveText::actionName);
    btMainWindow()->toolsToolBar()->addAction(action);
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::deleteEntry::actionName);
    btMainWindow()->toolsToolBar()->addAction(action);
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::restoreText::actionName);
    btMainWindow()->toolsToolBar()->addAction(action);
}

void CPlainWriteWindow::initConnections() {
    CWriteWindow::initConnections();
    QObject::connect(keyChooser(), SIGNAL(keyChanged(CSwordKey*)), this, SLOT(lookupSwordKey(CSwordKey*)));
    QObject::connect(displayWidget()->connectionsProxy(), SIGNAL(textChanged()), this, SLOT(textChanged()) );
}

void CPlainWriteWindow::storeProfileSettings( CProfileWindow* profileWindow ) {
    CWriteWindow::storeProfileSettings(profileWindow);
    QAction* action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    profileWindow->setWindowSettings( action->isChecked() );
}

void CPlainWriteWindow::applyProfileSettings( CProfileWindow* profileWindow ) {
    CWriteWindow::applyProfileSettings(profileWindow);
    if (profileWindow->windowSettings()) {
        QAction* action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
        action->setChecked(true);
    }
}

/** Saves the text for the current key. Directly writes the changed text into the module. */
void CPlainWriteWindow::saveCurrentText( const QString& /*key*/ ) {
    QString t = ((CWriteDisplay*)displayWidget())->plainText();
    //since t is a complete HTML page at the moment, strip away headers and footers of a HTML page
    QRegExp re("(?:<html.*>.+<body.*>)", Qt::CaseInsensitive); //remove headers, case insensitive
    re.setMinimal(true);
    t.replace(re, "");
    t.replace(QRegExp("</body></html>", Qt::CaseInsensitive), "");//remove footer

    const QString& oldKey = this->key()->key();
    if ( modules().first()->isWritable() ) {
        const_cast<CSwordModuleInfo*>(modules().first())->write(this->key(), t);
        this->key()->setKey(oldKey);

        ((CWriteDisplay*)displayWidget())->setModified(false);
        textChanged();
    }
    else {
        util::showCritical( this, tr("Module not writable"),
                            QString::fromLatin1("<qt><b>%1</b><br/>%2</qt>")
                            .arg( tr("Module is not writable.") )
                            .arg( tr("Either the module may not be edited, or "
                                     "you do not have write permission.") ) );
    }
}

/** Loads the original text from the module. */
void CPlainWriteWindow::restoreText() {
    lookupSwordKey(key());
    ((CWriteDisplay*)displayWidget())->setModified(false);
    textChanged();
}

/** Is called when the current text was changed. */
void CPlainWriteWindow::textChanged() {
    QAction* action = actionCollection()->action(CResMgr::displaywindows::writeWindow::saveText::actionName);
    action->setEnabled( ((CWriteDisplay*)displayWidget())->isModified() );
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::restoreText::actionName);
    action->setEnabled( ((CWriteDisplay*)displayWidget())->isModified() );
}

/** Deletes the module entry and clears the edit widget, */
void CPlainWriteWindow::deleteEntry() {
    const_cast<CSwordModuleInfo*>(modules().first())->deleteEntry(key());
    lookupSwordKey( key() );
    ((CWriteDisplay*)displayWidget())->setModified(false);
}

/** Setups the popup menu of this display widget. */
void CPlainWriteWindow::setupPopupMenu() {}

bool CPlainWriteWindow::syncAllowed() const {
    QAction* action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    return action->isChecked();
}

void CPlainWriteWindow::initActions() {
    insertKeyboardActions(actionCollection());

    QAction* action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    bool ok = QObject::connect(action, SIGNAL(triggered()), this, SLOT(saveCurrentText()));
    Q_ASSERT(ok);

    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::saveText::actionName);
    ok = QObject::connect(action, SIGNAL(triggered()), this, SLOT(saveCurrentText()));
    Q_ASSERT(ok);

    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::deleteEntry::actionName);
    ok = QObject::connect(action, SIGNAL(triggered()), this, SLOT(deleteEntry()) );
    Q_ASSERT(ok);

    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::restoreText::actionName);
    ok = QObject::connect(action, SIGNAL(triggered()), this, SLOT(restoreText()) );
    Q_ASSERT(ok);
}

void CPlainWriteWindow::insertKeyboardActions( BtActionCollection* const a) {

    namespace DU = util::directory;

    QAction* action = new QAction(
        DU::getIcon(CResMgr::displaywindows::commentaryWindow::syncWindow::icon),
        tr("Sync with active Bible"),
        a
    );
    action->setCheckable(true);
    action->setShortcut(CResMgr::displaywindows::commentaryWindow::syncWindow::accel);
    action->setToolTip(tr("Synchronize (show the same verse) with the active Bible window"));
    a->addAction(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName, action);

    action = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::saveText::icon),
        tr("Save text"),
        a
    );
    action->setShortcut(CResMgr::displaywindows::writeWindow::saveText::accel);
    action->setToolTip( tr("Save text") );
    a->addAction(CResMgr::displaywindows::writeWindow::saveText::actionName, action);

    action = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::deleteEntry::icon),
        tr("Delete current entry"),
        a
    );
    action->setShortcut(CResMgr::displaywindows::writeWindow::deleteEntry::accel);
    action->setToolTip( tr("Delete current entry (no undo)") );
    a->addAction(CResMgr::displaywindows::writeWindow::deleteEntry::actionName, action);

    action = new QAction(
        DU::getIcon(CResMgr::displaywindows::writeWindow::restoreText::icon),
        tr("Restore original text"),
        a
    );
    action->setShortcut(CResMgr::displaywindows::writeWindow::restoreText::accel);
    action->setToolTip( tr("Restore original text, new text will be lost") );
    a->addAction(CResMgr::displaywindows::writeWindow::restoreText::actionName, action);
}
