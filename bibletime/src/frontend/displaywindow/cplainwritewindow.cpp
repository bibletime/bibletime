/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cplainwritewindow.h"
#include "cplainwritewindow.moc"

#include "frontend/display/cwritedisplay.h"

#include "frontend/keychooser/ckeychooser.h"
#include "frontend/profile/cprofilewindow.h"
#include "frontend/cbtconfig.h"

#include "util/cresmgr.h"

#include <QRegExp>
#include <QToolBar>
#include <QMessageBox>
#include <QDebug>

#include <kaction.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <klocale.h>

using namespace Profile;

CPlainWriteWindow::CPlainWriteWindow(ListCSwordModuleInfo moduleList, CMDIArea* parent) : 
	CWriteWindow(moduleList, parent)
{
	setKey( CSwordKey::createInstance(moduleList.first()) );
}


CPlainWriteWindow::~CPlainWriteWindow() {}

/** Initialize the state of this widget. */
void CPlainWriteWindow::initView() {
	//  qWarning("CPlainWriteWindow::initView()");
	setDisplayWidget( CDisplay::createWriteInstance(this) );
	setCentralWidget( displayWidget()->view() );

	setMainToolBar( new QToolBar(this) );
	addToolBar(mainToolBar());

	setKeyChooser( CKeyChooser::createInstance(modules(), key(), mainToolBar()) );
	mainToolBar()->addWidget(keyChooser());
}

void CPlainWriteWindow::initToolbars() {
	m_actions.syncWindow = new KToggleAction(
			KIcon(CResMgr::displaywindows::commentaryWindow::syncWindow::icon),
			i18n("Sync with active Bible"),
			actionCollection()
			);
	m_actions.syncWindow->setShortcut(CResMgr::displaywindows::commentaryWindow::syncWindow::accel);
	m_actions.syncWindow->setToolTip(CResMgr::displaywindows::commentaryWindow::syncWindow::tooltip);
	mainToolBar()->addAction(m_actions.syncWindow);
	actionCollection()->addAction(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName, m_actions.syncWindow);


	m_actions.saveText = new KAction(
			KIcon(CResMgr::displaywindows::writeWindow::saveText::icon),
			i18n("Save text"),
			actionCollection()
			);
	m_actions.saveText->setShortcut(CResMgr::displaywindows::writeWindow::saveText::accel);
	QObject::connect(m_actions.saveText, SIGNAL(triggered()), this, SLOT(saveCurrentText()));
	m_actions.saveText->setToolTip( CResMgr::displaywindows::writeWindow::saveText::tooltip );
	actionCollection()->addAction(CResMgr::displaywindows::writeWindow::saveText::actionName, m_actions.saveText);
	mainToolBar()->addAction(m_actions.saveText);


	m_actions.deleteEntry = new KAction(
			KIcon(CResMgr::displaywindows::writeWindow::deleteEntry::icon),
			i18n("Delete current entry"),
			actionCollection()
			);
	m_actions.deleteEntry->setShortcut(CResMgr::displaywindows::writeWindow::deleteEntry::accel);
	QObject::connect(m_actions.deleteEntry, SIGNAL(triggered()), this, SLOT(deleteEntry()) );
	m_actions.deleteEntry->setToolTip( CResMgr::displaywindows::writeWindow::deleteEntry::tooltip );
	actionCollection()->addAction(CResMgr::displaywindows::writeWindow::deleteEntry::actionName, m_actions.deleteEntry);
	mainToolBar()->addAction(m_actions.deleteEntry);


	m_actions.restoreText = new KAction(
			KIcon(CResMgr::displaywindows::writeWindow::restoreText::icon),
			i18n("Restore original text"),
			actionCollection()
			);
	m_actions.restoreText->setShortcut(CResMgr::displaywindows::writeWindow::restoreText::accel);
	QObject::connect(m_actions.restoreText, SIGNAL(triggered()), this, SLOT(restoreText()) );
	m_actions.restoreText->setToolTip( CResMgr::displaywindows::writeWindow::restoreText::tooltip );
	actionCollection()->addAction(CResMgr::displaywindows::writeWindow::restoreText::actionName, m_actions.restoreText);
	mainToolBar()->addAction(m_actions.restoreText);
}

void CPlainWriteWindow::initConnections() {
	CWriteWindow::initConnections();
	QObject::connect(keyChooser(), SIGNAL(keyChanged(CSwordKey*)), this, SLOT(lookup(CSwordKey*)));
	QObject::connect(displayWidget()->connectionsProxy(), SIGNAL(textChanged()), this, SLOT(textChanged()) );
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
	QRegExp re("(?:<html.*>.+<body.*>)", Qt::CaseInsensitive); //remove headers, case insensitive
	re.setMinimal(true);
	t.replace(re, "");
	t.replace(QRegExp("</body></html>", Qt::CaseInsensitive), "");//remove footer

	const QString& oldKey = this->key()->key();
	if( modules().first()->isWritable() ) {
		modules().first()->write(this->key(), t );
		this->key()->key( oldKey );

		displayWidget()->setModified(false);
		textChanged();
	}
	else {
		QMessageBox::critical( this, i18n("Module not writable"),
			QString::fromLatin1("<qt><B>%1</B><BR>%2</qt>")
			.arg( i18n("Module is not writable.") )
			.arg( i18n("Either the module may not be edited, or "
					"you do not have write permission.") ) );
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
