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
#include "bibletimeapp.h"
#include "frontend/display/cplainwritedisplay.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/keychooser/ckeychooser.h"
#include "util/btsignal.h"
#include "util/cresmgr.h"
#include "util/dialogutil.h"


CPlainWriteWindow::CPlainWriteWindow(const QList<CSwordModuleInfo*> & moduleList, CMDIArea * parent)
    : CDisplayWindow(moduleList, parent)
    , m_writeDisplay(0)
{
    setKey( CSwordKey::createInstance(moduleList.first()) );
}

void CPlainWriteWindow::setDisplayWidget(CDisplay * display) {
    Q_ASSERT(dynamic_cast<CPlainWriteDisplay *>(display));
    CDisplayWindow::setDisplayWidget(static_cast<CPlainWriteDisplay *>(display));
    m_writeDisplay = static_cast<CPlainWriteDisplay *>(display);
}

void CPlainWriteWindow::lookupSwordKey(CSwordKey * newKey) {
    //set the raw text to the display widget
    if (!newKey)
        return;

    /*
      Set passage of newKey to key() if they're different, otherwise we'd get
      mixed up if we look up newkey which may have a different module set.
    */
    if (key() != newKey)
        key()->setKey(newKey->key());

    if (modules().count())
        displayWidget()->setText(key()->rawText());

    setWindowTitle(windowCaption());
}

/** Initialize the state of this widget. */
void CPlainWriteWindow::initView() {
    //  qWarning("CPlainWriteWindow::initView()");
    m_writeDisplay = new CPlainWriteDisplay(this, this);
    setDisplayWidget(m_writeDisplay);
    setCentralWidget(m_writeDisplay->view());

    // Create Navigation toolbar
    setMainToolBar( new QToolBar(this) );
    addToolBar(mainToolBar());

    // Create the Tools toolbar
    setButtonsToolBar( new QToolBar(this) );
    addToolBar(buttonsToolBar());
}

void CPlainWriteWindow::initToolbars() {
    // Navigation toolbar
    setKeyChooser( CKeyChooser::createInstance(modules(),
        history(), key(), mainToolBar()) );
    mainToolBar()->addWidget(keyChooser());

    // Tools toolbar
    QAction* action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    Q_ASSERT(action != 0);
    buttonsToolBar()->addAction(action);
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::saveText::actionName);
    Q_ASSERT(action != 0);
    buttonsToolBar()->addAction(action);
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::deleteEntry::actionName);
    Q_ASSERT(action != 0);
    buttonsToolBar()->addAction(action);
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::restoreText::actionName);
    Q_ASSERT(action != 0);
    buttonsToolBar()->addAction(action);
}

void CPlainWriteWindow::setupMainWindowToolBars() {
    // Navigation toolbar
    CKeyChooser* keyChooser = CKeyChooser::createInstance(modules(), history(), key(), btMainWindow()->navToolBar() );
    btMainWindow()->navToolBar()->addWidget(keyChooser);
    bool ok = connect(keyChooser, SIGNAL(keyChanged(CSwordKey*)), this, SLOT(lookupSwordKey(CSwordKey*)));
    Q_ASSERT(ok);

    // Tools toolbar
    QAction* action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    Q_ASSERT(action != 0);
    btMainWindow()->toolsToolBar()->addAction(action);
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::saveText::actionName);
    Q_ASSERT(action != 0);
    btMainWindow()->toolsToolBar()->addAction(action);
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::deleteEntry::actionName);
    Q_ASSERT(action != 0);
    btMainWindow()->toolsToolBar()->addAction(action);
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::restoreText::actionName);
    Q_ASSERT(action != 0);
    btMainWindow()->toolsToolBar()->addAction(action);
}

void CPlainWriteWindow::initConnections() {
    Q_ASSERT(keyChooser());
    QObject::connect(key()->beforeChangedSignaller(), SIGNAL(signal()), this, SLOT(beforeKeyChange()));
    QObject::connect(keyChooser(), SIGNAL(keyChanged(CSwordKey*)), this, SLOT(lookupSwordKey(CSwordKey*)));
    QObject::connect(displayWidget()->connectionsProxy(), SIGNAL(textChanged()), this, SLOT(textChanged()) );
}

void CPlainWriteWindow::storeProfileSettings(const QString & windowGroup) {
    CDisplayWindow::storeProfileSettings(windowGroup);

    QAction * action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    Q_ASSERT(action);
    Q_ASSERT(windowGroup.endsWith('/'));
    btConfig().setSessionValue(windowGroup + "writeWindowType",
                               static_cast<int>(writeWindowType()));
    btConfig().setSessionValue(windowGroup + "syncWindowEnabled", action->isChecked());
}

void CPlainWriteWindow::applyProfileSettings(const QString & windowGroup) {
    CDisplayWindow::applyProfileSettings(windowGroup);

    QAction * action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    Q_ASSERT(action != 0);
    Q_ASSERT(windowGroup.endsWith('/'));
    action->setChecked(btConfig().sessionValue<bool>(windowGroup + "syncWindowEnabled", false));
}

/** Saves the text for the current key. Directly writes the changed text into the module. */
void CPlainWriteWindow::saveCurrentText( const QString& /*key*/ ) {
    QString t = m_writeDisplay->plainText();
    //since t is a complete HTML page at the moment, strip away headers and footers of a HTML page
    QRegExp re("(?:<html.*>.+<body.*>)", Qt::CaseInsensitive); //remove headers, case insensitive
    re.setMinimal(true);
    t.replace(re, "");
    t.replace(QRegExp("</body></html>", Qt::CaseInsensitive), "");//remove footer

    const QString& oldKey = this->key()->key();
    if ( modules().first()->isWritable() ) {
        const_cast<CSwordModuleInfo*>(modules().first())->write(this->key(), t);
        m_writeDisplay->setModified(false);
        this->key()->setKey(oldKey);
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
    m_writeDisplay->setModified(false);
    textChanged();
}

/** Is called when the current text was changed. */
void CPlainWriteWindow::textChanged() {
    QAction* action = actionCollection()->action(CResMgr::displaywindows::writeWindow::saveText::actionName);
    Q_ASSERT(action != 0);
    action->setEnabled(m_writeDisplay->isModified());
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::restoreText::actionName);
    Q_ASSERT(action != 0);
    action->setEnabled(m_writeDisplay->isModified());
}

/** Deletes the module entry and clears the edit widget, */
void CPlainWriteWindow::deleteEntry() {
    const_cast<CSwordModuleInfo*>(modules().first())->deleteEntry(key());
    lookupSwordKey( key() );
    m_writeDisplay->setModified(false);
}

/** Setups the popup menu of this display widget. */
void CPlainWriteWindow::setupPopupMenu() {}

bool CPlainWriteWindow::syncAllowed() const {
    QAction* action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    Q_ASSERT(action != 0);
    return action->isChecked();
}

void CPlainWriteWindow::initActions() {
    insertKeyboardActions(actionCollection());

    QAction* action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    Q_ASSERT(action != 0);
    bool ok = QObject::connect(action, SIGNAL(triggered()),
                               this,   SLOT(saveCurrentText()));
    Q_ASSERT(ok);

    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::saveText::actionName);
    Q_ASSERT(action != 0);
    ok = QObject::connect(action, SIGNAL(triggered()),
                          this,   SLOT(saveCurrentText()));
    Q_ASSERT(ok);

    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::deleteEntry::actionName);
    Q_ASSERT(action != 0);
    ok = QObject::connect(action, SIGNAL(triggered()),
                          this,   SLOT(deleteEntry()));
    Q_ASSERT(ok);

    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::restoreText::actionName);
    Q_ASSERT(action != 0);
    ok = QObject::connect(action, SIGNAL(triggered()),
                          this,   SLOT(restoreText()));
    Q_ASSERT(ok);
}

void CPlainWriteWindow::insertKeyboardActions( BtActionCollection* const a) {
    QAction* action = new QAction(
        bApp->getIcon(CResMgr::displaywindows::commentaryWindow::syncWindow::icon),
        tr("Sync with active Bible"),
        a
    );
    action->setCheckable(true);
    action->setShortcut(CResMgr::displaywindows::commentaryWindow::syncWindow::accel);
    action->setToolTip(tr("Synchronize (show the same verse) with the active Bible window"));
    a->addAction(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName, action);

    action = new QAction(
        bApp->getIcon(CResMgr::displaywindows::writeWindow::saveText::icon),
        tr("Save text"),
        a
    );
    action->setShortcut(CResMgr::displaywindows::writeWindow::saveText::accel);
    action->setToolTip( tr("Save text") );
    a->addAction(CResMgr::displaywindows::writeWindow::saveText::actionName, action);

    action = new QAction(
        bApp->getIcon(CResMgr::displaywindows::writeWindow::deleteEntry::icon),
        tr("Delete current entry"),
        a
    );
    action->setShortcut(CResMgr::displaywindows::writeWindow::deleteEntry::accel);
    action->setToolTip( tr("Delete current entry (no undo)") );
    a->addAction(CResMgr::displaywindows::writeWindow::deleteEntry::actionName, action);

    action = new QAction(
        bApp->getIcon(CResMgr::displaywindows::writeWindow::restoreText::icon),
        tr("Restore original text"),
        a
    );
    action->setShortcut(CResMgr::displaywindows::writeWindow::restoreText::accel);
    action->setToolTip( tr("Restore original text, new text will be lost") );
    a->addAction(CResMgr::displaywindows::writeWindow::restoreText::actionName, action);
}

void CPlainWriteWindow::saveCurrentText() {
    if (key())
        saveCurrentText(key()->key());
}


bool CPlainWriteWindow::queryClose() {
    //save the text if it has changed
    if (m_writeDisplay->isModified()) {
        switch (util::showQuestion( this, tr("Save Text?"), tr("Save text before closing?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes) ) {
            case QMessageBox::Yes: //save and close
                saveCurrentText();
                m_writeDisplay->setModified( false );
                return true;
            case QMessageBox::No: //don't save and close
                return true;
            default: // cancel, don't close
                return false;
        }
    }
    return true;
}

void CPlainWriteWindow::beforeKeyChange() {
    Q_ASSERT(displayWidget());
    Q_ASSERT(keyChooser());
    if (!isReady())
        return;

    // Get current key string for this window
    QString thisWindowsKey;
    CSwordKey* oldKey = key();
    if (oldKey == 0)
        return;
    thisWindowsKey = oldKey->key();

    //If the text changed and we'd do a lookup ask the user if the text should be saved
    if (modules().first() && m_writeDisplay->isModified()) {

        switch (util::showQuestion( this, tr("Save Text?"), tr("Save changed text?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) ) {
            case QMessageBox::Yes: { //save the changes
                saveCurrentText( thisWindowsKey );
                break;
            }
            default: {// set modified to false so it won't ask again
                m_writeDisplay->setModified(false);
                break;
            }
        }
    }
}
