/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "cplainwritewindow.h"

#include "frontend/keychooser/ckeychooser.h"
#include "frontend/cprofilewindow.h"
#include "frontend/cbtconfig.h"

#include "util/cresmgr.h"

//Qt includes
#include <qregexp.h>

//KDE includes
#include <kaction.h>
#include <kaccel.h>
#include <klocale.h>
#include <kmessagebox.h>

using namespace Profile;

CPlainWriteWindow::CPlainWriteWindow(ListCSwordModuleInfo moduleList, CMDIArea* parent, const char *name ) : CWriteWindow(moduleList, parent, name) {
	setKey( CSwordKey::createInstance(moduleList.first()) );
}


CPlainWriteWindow::~CPlainWriteWindow() {}

/** Initialize the state of this widget. */
void CPlainWriteWindow::initView() {
	//  qWarning("CPlainWriteWindow::initView()");
	setDisplayWidget( CDisplay::createWriteInstance(this) );
	setCentralWidget( displayWidget()->view() );

	setMainToolBar( new KToolBar(this) );
	mainToolBar()->setFullSize(true);
	addDockWindow(mainToolBar());

	setKeyChooser( CKeyChooser::createInstance(modules(), key(), mainToolBar()) );
	mainToolBar()->insertWidget(0,keyChooser()->sizeHint().width(),keyChooser());
	mainToolBar()->setFullSize(false);

}

void CPlainWriteWindow::initToolbars() {
	m_actions.syncWindow = new KToggleAction(i18n("Sync with active Bible"),
						   CResMgr::displaywindows::commentaryWindow::syncWindow::icon,
						   CResMgr::displaywindows::commentaryWindow::syncWindow::accel,
						   actionCollection(),
						   CResMgr::displaywindows::commentaryWindow::syncWindow::actionName
											);
	m_actions.syncWindow->setToolTip(CResMgr::displaywindows::commentaryWindow::syncWindow::tooltip);
	m_actions.syncWindow->plug(mainToolBar());


	m_actions.saveText = new KAction(i18n("Save text"),
									 CResMgr::displaywindows::writeWindow::saveText::icon,
									 CResMgr::displaywindows::writeWindow::saveText::accel,
									 this, SLOT(saveCurrentText()),
									 actionCollection(),
									 CResMgr::displaywindows::writeWindow::saveText::actionName
									);
	m_actions.saveText->setToolTip( CResMgr::displaywindows::writeWindow::saveText::tooltip );
	m_actions.saveText->plug(mainToolBar());


	m_actions.deleteEntry = new KAction(i18n("Delete current entry"),
										CResMgr::displaywindows::writeWindow::deleteEntry::icon,
										CResMgr::displaywindows::writeWindow::deleteEntry::accel,
										this, SLOT(deleteEntry()),
										actionCollection(),
										CResMgr::displaywindows::writeWindow::deleteEntry::actionName
									   );
	m_actions.deleteEntry->setToolTip( CResMgr::displaywindows::writeWindow::deleteEntry::tooltip );
	m_actions.deleteEntry->plug(mainToolBar());


	m_actions.restoreText = new KAction(i18n("Restore original text"),
										CResMgr::displaywindows::writeWindow::restoreText::icon,
										CResMgr::displaywindows::writeWindow::restoreText::accel,
										this, SLOT(restoreText()),
										actionCollection(),
										CResMgr::displaywindows::writeWindow::restoreText::actionName
									   );
	m_actions.restoreText->setToolTip( CResMgr::displaywindows::writeWindow::restoreText::tooltip );
	m_actions.restoreText->plug(mainToolBar());
}

void CPlainWriteWindow::initConnections() {
	CWriteWindow::initConnections();
	connect(keyChooser(), SIGNAL(keyChanged(CSwordKey*)),
			this, SLOT(lookup(CSwordKey*)));

	connect(displayWidget()->connectionsProxy(), SIGNAL(textChanged()),
			this, SLOT(textChanged()) );
}

void CPlainWriteWindow::storeProfileSettings( CProfileWindow* profileWindow ) {
	CWriteWindow::storeProfileSettings(profileWindow);
	profileWindow->setWindowSettings( m_actions.syncWindow->isChecked() );
};

void CPlainWriteWindow::applyProfileSettings( CProfileWindow* profileWindow ) {
	CWriteWindow::applyProfileSettings(profileWindow);
	if (profileWindow->windowSettings()) {
		m_actions.syncWindow->setChecked(true);
	}
};

/** Saves the text for the current key. Directly writes the changed text into the module. */
void CPlainWriteWindow::saveCurrentText( const QString& /*key*/ ) {
	QString t = displayWidget()->plainText();
	//since t is a complete HTML page at the moment, strip away headers and footers of a HTML page
	QRegExp re("(?:<html.*>.+<body.*>)", false); //remove headers, case insensitive
	re.setMinimal(true);
	t.replace(re, "");
	t.replace(QRegExp("</body></html>", false), "");//remove footer

	const QString& oldKey = this->key()->key();
	if( modules().first()->isWritable() ) {
		modules().first()->write(this->key(), t );
		this->key()->key( oldKey );

		displayWidget()->setModified(false);
		textChanged();
	} else {
		KMessageBox::error( this,
				    QString::fromLatin1("<qt><B>%1</B><BR>%2</qt>")
				    .arg( i18n("Module is not writable.") )
				    .arg( i18n("Either the module may not be edited, or "
					       "you do not have write permission.") ),
				    i18n("Module not writable") );
	}
}

/** Loads the original text from the module. */
void CPlainWriteWindow::restoreText() {
	lookup(key());
	displayWidget()->setModified(false);
	textChanged();
}

/** Is called when the current text was changed. */
void CPlainWriteWindow::textChanged() {
	m_actions.saveText->setEnabled( displayWidget()->isModified() );
	m_actions.restoreText->setEnabled( displayWidget()->isModified() );
}

/** Deletes the module entry and clears the edit widget, */
void CPlainWriteWindow::deleteEntry() {
	modules().first()->deleteEntry( key() );
	lookup( key() );
	displayWidget()->setModified(false);
}

/** Setups the popup menu of this display widget. */
void CPlainWriteWindow::setupPopupMenu() {}

const bool CPlainWriteWindow::syncAllowed() const {
	return m_actions.syncWindow->isChecked();
}

void CPlainWriteWindow::initActions() {
}

void CPlainWriteWindow::insertKeyboardActions( KActionCollection* const ) {
}
