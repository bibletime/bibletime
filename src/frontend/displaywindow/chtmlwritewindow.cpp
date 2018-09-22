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

#include "frontend/displaywindow/chtmlwritewindow.h"

#include <QAction>
#include <QToolBar>
#include "backend/keys/cswordkey.h"
#include "frontend/bibletime.h"
#include "frontend/display/chtmlwritedisplay.h"
#include "frontend/displaywindow/btactioncollection.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/messagedialog.h"
#include "util/btassert.h"
#include "util/btconnect.h"
#include "util/directory.h"
#include "util/cresmgr.h"


CHTMLWriteWindow::CHTMLWriteWindow(const QList<CSwordModuleInfo *> & modules, CMDIArea * parent)
        : CPlainWriteWindow(modules, parent) {}

void CHTMLWriteWindow::initView() {
    m_writeDisplay = new CHTMLWriteDisplay(this, this);
    BT_ASSERT(m_writeDisplay);
    setDisplayWidget(m_writeDisplay);
    setCentralWidget(m_writeDisplay->view() );

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

void CHTMLWriteWindow::initConnections() {
    CPlainWriteWindow::initConnections();

    BT_CONNECT(keyChooser(), SIGNAL(keyChanged(CSwordKey *)),
               this,         SLOT(lookupSwordKey(CSwordKey *)));
    BT_CONNECT(m_writeDisplay->connectionsProxy(), SIGNAL(textChanged()),
               this,                               SLOT(textChanged()));
}

void CHTMLWriteWindow::initToolbars() {
    namespace DU = util::directory;

    CPlainWriteWindow::initToolbars();

    //Formatting toolbar
    m_writeDisplay->setupToolbar( formatToolBar(), actionCollection() );
}

void CHTMLWriteWindow::storeProfileSettings(QString const & windowGroup) const {
    CPlainWriteWindow::storeProfileSettings(windowGroup);

    BT_ASSERT(windowGroup.endsWith('/'));
    namespace SW = CResMgr::displaywindows::commentaryWindow::syncWindow;
    btConfig().setSessionValue(
            windowGroup + "syncWindowEnabled",
            actionCollection()->action(SW::actionName).isChecked());
}

void CHTMLWriteWindow::applyProfileSettings(const QString & windowGroup) {
    CPlainWriteWindow::applyProfileSettings(windowGroup);

    namespace SW = CResMgr::displaywindows::commentaryWindow::syncWindow;
    BT_ASSERT(windowGroup.endsWith('/'));
    actionCollection()->action(SW::actionName).setChecked(
            btConfig().sessionValue<bool>(windowGroup + "syncWindowEnabled",
                                          false));
}

/** Is called when the current text was changed. */
void CHTMLWriteWindow::textChanged() {
    namespace WW = CResMgr::displaywindows::writeWindow;
    actionCollection()->action(WW::saveText::actionName)
            .setEnabled(m_writeDisplay->isModified());
    actionCollection()->action(WW::restoreText::actionName)
            .setEnabled(m_writeDisplay->isModified());
}

/** Loads the original text from the module. */
void CHTMLWriteWindow::restoreText() {
    lookupSwordKey(key());
    m_writeDisplay->setModified(false);
    textChanged();
}

bool CHTMLWriteWindow::syncAllowed() const {
    return actionCollection()->action(
            CResMgr::displaywindows::commentaryWindow::syncWindow::actionName)
                .isChecked();
}

/** Saves the text for the current key. Directly writes the changed text into the module. */
void CHTMLWriteWindow::saveCurrentText( const QString& /*key*/ ) {
    QString t = m_writeDisplay->toHtml();
    //since t is a complete HTML page at the moment, strip away headers and footers of a HTML page
    QRegExp re("(?:<html.*>.+<body.*>)", Qt::CaseInsensitive); //remove headers, case insensitive
    re.setMinimal(true);
    t.replace(re, "");
    t.replace(QRegExp("</body></html>", Qt::CaseInsensitive), "");//remove footer

    const QString& oldKey = this->key()->key();
    if ( modules().first()->isWritable() ) {
        const_cast<CSwordModuleInfo*>(modules().first())->write(this->key(), t);
        this->key()->setKey(oldKey);

        m_writeDisplay->setModified(false);
        textChanged();
    }
    else {
        message::showCritical( this, tr("Module not writable"),
                            QString::fromLatin1("<qt><b>%1</b><br />%2</qt>")
                            .arg( tr("Module is not writable.") )
                            .arg( tr("Either the module may not be edited, or you do not have write permission.") ) );
    }
}

void CHTMLWriteWindow::setupMainWindowToolBars() {
    CPlainWriteWindow::setupMainWindowToolBars();
    //Formatting toolbar
    m_writeDisplay->setupToolbar( btMainWindow()->formatToolBar(), actionCollection() );
}
