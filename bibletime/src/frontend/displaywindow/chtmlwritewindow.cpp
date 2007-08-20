/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2007 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "chtmlwritewindow.h"
#include "chtmlwritewindow.moc"

//frontend includes
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/profile/cprofilewindow.h"
#include "frontend/display/cwritedisplay.h"

#include "util/cresmgr.h"

//Qt includes

//KDE includes
#include <kaction.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <klocale.h>
#include <ktoolbar.h>

using namespace Profile;

CHTMLWriteWindow::CHTMLWriteWindow(ListCSwordModuleInfo modules, CMDIArea* parent)
: CPlainWriteWindow(modules, parent) {}

CHTMLWriteWindow::~CHTMLWriteWindow() {}

void CHTMLWriteWindow::initView() {
	CWriteDisplay* writeDisplay = CDisplay::createWriteInstance(this, CDisplay::HTMLDisplay);
	Q_ASSERT(writeDisplay);
	setDisplayWidget( writeDisplay );
	setWidget( displayWidget()->view() );

	setMainToolBar( new KToolBar(this) );
	//mainToolBar()->setFullSize(true);
	//this->addToolBarBreak(); // to replace setFullSize of Qt3?
	//TODO: Qt4 is different; I just remove the docking capability now
	//addDockWindow(mainToolBar());

	setKeyChooser( CKeyChooser::createInstance(modules(), key(), mainToolBar()) );
	mainToolBar()->addWidget(keyChooser());
	//mainToolBar()->setFullSize(false); //why?
};

void CHTMLWriteWindow::initConnections() {
	CWriteWindow::initConnections();

	connect(keyChooser(), SIGNAL(keyChanged(CSwordKey*)),
			this, SLOT(lookup(CSwordKey*)));
	connect(displayWidget()->connectionsProxy(), SIGNAL(textChanged()),
			this, SLOT(textChanged()) );
};

void CHTMLWriteWindow::initToolbars() {
	//setup the toolbar
	m_actions.syncWindow = new KToggleAction(
			KIcon(CResMgr::displaywindows::commentaryWindow::syncWindow::icon),
			i18n("Sync with active Bible"),
			actionCollection()
			);
	m_actions.syncWindow->setShortcut(CResMgr::displaywindows::commentaryWindow::syncWindow::accel);
	m_actions.syncWindow->setToolTip(CResMgr::displaywindows::commentaryWindow::syncWindow::tooltip);
	actionCollection()->addAction(CResMgr::displaywindows::commentaryWindow::syncWindow::actionName, m_actions.syncWindow);
	mainToolBar()->addAction(m_actions.syncWindow);

	m_actions.saveText = new KAction( 
			KIcon(CResMgr::displaywindows::writeWindow::saveText::icon),
			i18n("Save text"),
			actionCollection()
			);
	m_actions.saveText->setShortcut(CResMgr::displaywindows::writeWindow::saveText::accel);
	m_actions.saveText->setToolTip( CResMgr::displaywindows::writeWindow::saveText::tooltip );
	QObject::connect(m_actions.saveText, SIGNAL(triggered()), this, SLOT( saveCurrentText() ) );
	actionCollection()->addAction(CResMgr::displaywindows::writeWindow::saveText::actionName, m_actions.saveText);
	mainToolBar()->addAction(m_actions.saveText);
	

	m_actions.deleteEntry = new KAction(
			KIcon(CResMgr::displaywindows::writeWindow::deleteEntry::icon),
			i18n("Delete current entry"),
			actionCollection()
			);
	m_actions.deleteEntry->setShortcut(CResMgr::displaywindows::writeWindow::deleteEntry::accel);
	m_actions.deleteEntry->setToolTip( CResMgr::displaywindows::writeWindow::deleteEntry::tooltip );
	QObject::connect(m_actions.deleteEntry, SIGNAL(triggered()), this, SLOT( deleteEntry() ) );
	actionCollection()->addAction(CResMgr::displaywindows::writeWindow::deleteEntry::actionName, m_actions.deleteEntry);
	mainToolBar()->addAction(m_actions.deleteEntry);

	m_actions.restoreText = new KAction(
			KIcon(CResMgr::displaywindows::writeWindow::restoreText::icon),
			i18n("Restore original text"),
			actionCollection()
			);
	m_actions.restoreText->setShortcut(CResMgr::displaywindows::writeWindow::restoreText::accel);
	m_actions.restoreText->setToolTip( CResMgr::displaywindows::writeWindow::restoreText::tooltip );
	QObject::connect(m_actions.restoreText, SIGNAL(triggered()), this, SLOT( restoreText() ) );
	actionCollection()->addAction(CResMgr::displaywindows::writeWindow::restoreText::actionName, m_actions.restoreText);
	mainToolBar()->addAction(m_actions.restoreText);


	KToolBar* bar = new KToolBar(this);
	//bar->setFullSize(true);
	//addDockWindow(bar); //see cplainwritewindow

	displayWidget()->setupToolbar( bar, actionCollection() );
}

void CHTMLWriteWindow::storeProfileSettings( CProfileWindow* profileWindow ) {
	CWriteWindow::storeProfileSettings(profileWindow);
	profileWindow->setWindowSettings( m_actions.syncWindow->isChecked() );
};

void CHTMLWriteWindow::applyProfileSettings( CProfileWindow* profileWindow ) {
	CWriteWindow::applyProfileSettings(profileWindow);
	if (profileWindow->windowSettings()) {
		m_actions.syncWindow->setChecked(true);
	}
};

/** Is called when the current text was changed. */
void CHTMLWriteWindow::textChanged() {
	m_actions.saveText->setEnabled( displayWidget()->isModified() );
	m_actions.restoreText->setEnabled( displayWidget()->isModified() );
}

/** Loads the original text from the module. */
void CHTMLWriteWindow::restoreText() {
	lookup(key());
	displayWidget()->setModified(false);
	textChanged();
}

const bool CHTMLWriteWindow::syncAllowed() const {
	return m_actions.syncWindow->isChecked();
}
