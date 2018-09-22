/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2018 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License
* version 2.0.
*
**********/

#include "frontend/displaywindow/cplainwritewindow.h"

#include <QAction>
#include <QDebug>
#include <QRegExp>
#include <QToolBar>
#include "backend/keys/cswordkey.h"
#include "frontend/bibletime.h"
#include "frontend/bibletimeapp.h"
#include "frontend/display/cplainwritedisplay.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/messagedialog.h"
#include "util/btassert.h"
#include "util/btconnect.h"
#include "util/cresmgr.h"


CPlainWriteWindow::CPlainWriteWindow(const QList<CSwordModuleInfo*> & moduleList, CMDIArea * parent)
    : CDisplayWindow(moduleList, parent)
    , m_writeDisplay(nullptr)
{
    setKey( CSwordKey::createInstance(moduleList.first()) );
}

void CPlainWriteWindow::setDisplayWidget(CDisplay * display) {
    BT_ASSERT(dynamic_cast<CPlainWriteDisplay *>(display));
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
    auto const initAction = [this](QString const & actionName) {
        buttonsToolBar()->addAction(&actionCollection()->action(actionName));
    };
    using namespace CResMgr::displaywindows;
    initAction(commentaryWindow::syncWindow::actionName);
    initAction(writeWindow::saveText::actionName);
    initAction(writeWindow::deleteEntry::actionName);
    initAction(writeWindow::restoreText::actionName);
}

void CPlainWriteWindow::setupMainWindowToolBars() {
    // Navigation toolbar
    CKeyChooser* keyChooser = CKeyChooser::createInstance(modules(), history(), key(), btMainWindow()->navToolBar() );
    btMainWindow()->navToolBar()->addWidget(keyChooser);
    BT_CONNECT(keyChooser, SIGNAL(keyChanged(CSwordKey *)),
               this,       SLOT(lookupSwordKey(CSwordKey *)));

    // Tools toolbar
    QToolBar & toolsToolbar = *btMainWindow()->toolsToolBar();
    auto const initAction = [this, &toolsToolbar](QString const & actionName) {
        toolsToolbar.addAction(&actionCollection()->action(actionName));
    };
    using namespace CResMgr::displaywindows;
    initAction(commentaryWindow::syncWindow::actionName);
    initAction(writeWindow::saveText::actionName);
    initAction(writeWindow::deleteEntry::actionName);
    initAction(writeWindow::restoreText::actionName);
}

void CPlainWriteWindow::initConnections() {
    BT_ASSERT(keyChooser());
    BT_CONNECT(key()->beforeChangedSignaller(), SIGNAL(signal()),
               this,                            SLOT(beforeKeyChange()));
    BT_CONNECT(keyChooser(), SIGNAL(keyChanged(CSwordKey *)),
               this,         SLOT(lookupSwordKey(CSwordKey *)));
    BT_CONNECT(displayWidget()->connectionsProxy(), SIGNAL(textChanged()),
               this,                                SLOT(textChanged()));
}

void CPlainWriteWindow::storeProfileSettings(QString const & windowGroup) const {
    CDisplayWindow::storeProfileSettings(windowGroup);

    BT_ASSERT(windowGroup.endsWith('/'));
    btConfig().setSessionValue(windowGroup + "writeWindowType",
                               static_cast<int>(writeWindowType()));
    using namespace CResMgr::displaywindows;
    btConfig().setSessionValue(
            windowGroup + "syncWindowEnabled",
            actionCollection()->action(
                    commentaryWindow::syncWindow::actionName).isChecked());
}

void CPlainWriteWindow::applyProfileSettings(const QString & windowGroup) {
    CDisplayWindow::applyProfileSettings(windowGroup);

    BT_ASSERT(windowGroup.endsWith('/'));
    using namespace CResMgr::displaywindows;
    actionCollection()->action(commentaryWindow::syncWindow::actionName)
            .setChecked(
                    btConfig().sessionValue<bool>(
                            windowGroup + "syncWindowEnabled",
                            false));
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
        message::showCritical( this, tr("Module not writable"),
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
    namespace WW = CResMgr::displaywindows::writeWindow;
    auto const & ac = *actionCollection();
    ac.action(WW::saveText::actionName)
            .setEnabled(m_writeDisplay->isModified());
    ac.action(WW::restoreText::actionName)
            .setEnabled(m_writeDisplay->isModified());
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
    return actionCollection()->action(
            CResMgr::displaywindows::commentaryWindow::syncWindow::actionName)
                .isChecked();
}

void CPlainWriteWindow::initActions() {
    insertKeyboardActions(actionCollection());

    auto const initAction = [this](QString const & actionName,
                                   void (CPlainWriteWindow:: *slot)())
    {
        BT_CONNECT(&actionCollection()->action(actionName),
                   &QAction::triggered,
                   this, slot);
    };
    namespace DW = CResMgr::displaywindows;
    initAction(DW::commentaryWindow::syncWindow::actionName,
               &CPlainWriteWindow::saveCurrentText);
    initAction(DW::writeWindow::saveText::actionName,
               &CPlainWriteWindow::saveCurrentText);
    initAction(DW::writeWindow::deleteEntry::actionName,
               &CPlainWriteWindow::deleteEntry);
    initAction(DW::writeWindow::restoreText::actionName,
               &CPlainWriteWindow::restoreText);
}

void CPlainWriteWindow::insertKeyboardActions( BtActionCollection* const a) {
    QAction* action = new QAction(
        CResMgr::displaywindows::commentaryWindow::syncWindow::icon(),
        tr("Sync with active Bible"),
        a
    );
    action->setCheckable(true);
    action->setShortcut(CResMgr::displaywindows::commentaryWindow::syncWindow::accel);
    action->setToolTip(tr("Synchronize (show the same verse) with the active Bible window"));
    a->addAction(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName, action);

    action = new QAction(
        CResMgr::displaywindows::writeWindow::saveText::icon(),
        tr("Save text"),
        a
    );
    action->setShortcut(CResMgr::displaywindows::writeWindow::saveText::accel);
    action->setToolTip( tr("Save text") );
    a->addAction(CResMgr::displaywindows::writeWindow::saveText::actionName, action);

    action = new QAction(
        CResMgr::displaywindows::writeWindow::deleteEntry::icon(),
        tr("Delete current entry"),
        a
    );
    action->setShortcut(CResMgr::displaywindows::writeWindow::deleteEntry::accel);
    action->setToolTip( tr("Delete current entry (no undo)") );
    a->addAction(CResMgr::displaywindows::writeWindow::deleteEntry::actionName, action);

    action = new QAction(
        CResMgr::displaywindows::writeWindow::restoreText::icon(),
        tr("Restore original text"),
        a
    );
    action->setShortcut(CResMgr::displaywindows::writeWindow::restoreText::accel);
    action->setToolTip( tr("Restore original text, new text will be lost") );
    a->addAction(CResMgr::displaywindows::writeWindow::restoreText::actionName, action);
}

void CPlainWriteWindow::saveCurrentText() {
    if (key()) {
        m_writeDisplay->setModified(false);
        saveCurrentText(key()->key());
    }
}


bool CPlainWriteWindow::queryClose() {
    //save the text if it has changed
    if (m_writeDisplay->isModified()) {
        switch (message::showQuestion( this, tr("Save Text?"), tr("Save text before closing?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes) ) {
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
    BT_ASSERT(displayWidget());
    BT_ASSERT(keyChooser());
    if (!isReady())
        return;

    // Get current key string for this window
    QString thisWindowsKey;
    CSwordKey* oldKey = key();
    if (oldKey == nullptr)
        return;
    thisWindowsKey = oldKey->key();

    //If the text changed and we'd do a lookup ask the user if the text should be saved
    if (modules().first() && m_writeDisplay->isModified()) {

        switch (message::showQuestion( this, tr("Save Text?"), tr("Save changed text?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) ) {
            case QMessageBox::Yes: { //save the changes
                m_writeDisplay->setModified(false); // Do before saveCurrentText to prevent recursion
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
