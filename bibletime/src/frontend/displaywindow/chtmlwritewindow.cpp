/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2010 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/chtmlwritewindow.h"

#include <QAction>
#include <QToolBar>
#include "bibletime.h"
#include "backend/keys/cswordkey.h"
#include "frontend/display/chtmlwritedisplay.h"
#include "frontend/display/cwritedisplay.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/profile/cprofilewindow.h"
#include "util/directory.h"
#include "util/dialogutil.h"
#include "util/cresmgr.h"


using namespace Profile;

CHTMLWriteWindow::CHTMLWriteWindow(QList<CSwordModuleInfo*> modules, CMDIArea* parent)
        : CPlainWriteWindow(modules, parent) {}

CHTMLWriteWindow::~CHTMLWriteWindow() {}

void CHTMLWriteWindow::initView() {
    CWriteDisplay* writeDisplay = CDisplay::createWriteInstance(this, CDisplay::HTMLDisplay);
    Q_ASSERT(writeDisplay);
    setDisplayWidget( writeDisplay );
    setCentralWidget( displayWidget()->view() );

    // Create Navigation toolbar
    setMainToolBar( new QToolBar(this) );
    addToolBar(mainToolBar());

    // Create the Tools toolbar
    setButtonsToolBar( new QToolBar(this) );
    addToolBar(buttonsToolBar());

    // Create the Format toolbar
    setFormatToolBar( new QToolBar(this) );
    addToolBar(formatToolBar());
}

void CHTMLWriteWindow::initActions() {
    insertKeyboardActions(actionCollection());
    CPlainWriteWindow::initActions();
}

void CHTMLWriteWindow::insertKeyboardActions( BtActionCollection* const a) {
}

void CHTMLWriteWindow::initConnections() {
    CWriteWindow::initConnections();

    connect(keyChooser(), SIGNAL(keyChanged(CSwordKey*)), this, SLOT(lookupSwordKey(CSwordKey*)));
    connect(displayWidget()->connectionsProxy(), SIGNAL(textChanged()), this, SLOT(textChanged()) );
}

void CHTMLWriteWindow::initToolbars() {
    namespace DU = util::directory;

    CPlainWriteWindow::initToolbars();

    //Formatting toolbar
    ((CWriteDisplay*)displayWidget())->setupToolbar( formatToolBar(), actionCollection() );
}

void CHTMLWriteWindow::storeProfileSettings( CProfileWindow* profileWindow ) {
    CWriteWindow::storeProfileSettings(profileWindow);
    QAction* action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    profileWindow->setWindowSettings( action->isChecked() );
}

void CHTMLWriteWindow::applyProfileSettings( CProfileWindow* profileWindow ) {
    CWriteWindow::applyProfileSettings(profileWindow);
    if (profileWindow->windowSettings()) {
        QAction* action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
        action->setChecked(true);
    }
}

/** Is called when the current text was changed. */
void CHTMLWriteWindow::textChanged() {
    QAction* action = actionCollection()->action(CResMgr::displaywindows::writeWindow::saveText::actionName);
    action->setEnabled( ((CWriteDisplay*)displayWidget())->isModified() );
    action = actionCollection()->action(CResMgr::displaywindows::writeWindow::restoreText::actionName);
    action->setEnabled( ((CWriteDisplay*)displayWidget())->isModified() );
}

/** Loads the original text from the module. */
void CHTMLWriteWindow::restoreText() {
    lookupSwordKey(key());
    ((CWriteDisplay*)displayWidget())->setModified(false);
    textChanged();
}

bool CHTMLWriteWindow::syncAllowed() const {
    QAction* action = actionCollection()->action(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName);
    return action->isChecked();
}

/** Saves the text for the current key. Directly writes the changed text into the module. */
void CHTMLWriteWindow::saveCurrentText( const QString& /*key*/ ) {
    QString t = ((CHTMLWriteDisplay*)displayWidget())->toHtml();
    //since t is a complete HTML page at the moment, strip away headers and footers of a HTML page
    QRegExp re("(?:<html.*>.+<body.*>)", Qt::CaseInsensitive); //remove headers, case insensitive
    re.setMinimal(true);
    t.replace(re, "");
    t.replace(QRegExp("</body></html>", Qt::CaseInsensitive), "");//remove footer

    const QString& oldKey = this->key()->key();
    if ( modules().first()->isWritable() ) {
        modules().first()->write(this->key(), t );
        this->key()->key( oldKey );

        ((CWriteDisplay*)displayWidget())->setModified(false);
        textChanged();
    }
    else {
        util::showCritical( this, tr("Module not writable"),
                            QString::fromLatin1("<qt><b>%1</b><br />%2</qt>")
                            .arg( tr("Module is not writable.") )
                            .arg( tr("Either the module may not be edited, or you do not have write permission.") ) );
    }
}

void CHTMLWriteWindow::setupMainWindowToolBars() {
    CPlainWriteWindow::setupMainWindowToolBars();
    //Formatting toolbar
    ((CWriteDisplay*)displayWidget())->setupToolbar( btMainWindow()->formatToolBar(), actionCollection() );
}
