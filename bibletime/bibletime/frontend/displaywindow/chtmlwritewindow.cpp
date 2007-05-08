/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



#include "chtmlwritewindow.h"

//frontend includes
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/cprofilewindow.h"

#include "util/cresmgr.h"

//Qt includes

//KDE includes
#include <kaction.h>
#include <klocale.h>

using namespace Profile;

CHTMLWriteWindow::CHTMLWriteWindow(ListCSwordModuleInfo modules, CMDIArea* parent, const char *name)
: CPlainWriteWindow(modules, parent, name) {}

CHTMLWriteWindow::~CHTMLWriteWindow() {}

void CHTMLWriteWindow::initView() {
	CWriteDisplay* writeDisplay = CDisplay::createWriteInstance(this, CDisplay::HTMLDisplay);
	Q_ASSERT(writeDisplay);
	setDisplayWidget( writeDisplay );
	setCentralWidget( displayWidget()->view() );

	setMainToolBar( new KToolBar(this) );
	mainToolBar()->setFullSize(true);
	addDockWindow(mainToolBar());

	setKeyChooser( CKeyChooser::createInstance(modules(), key(), mainToolBar()) );
	mainToolBar()->insertWidget(0,keyChooser()->sizeHint().width(),keyChooser());
	mainToolBar()->setFullSize(false);
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
	m_actions.syncWindow = new KToggleAction(i18n("Sync with active Bible"),
						   CResMgr::displaywindows::commentaryWindow::syncWindow::icon,
						   CResMgr::displaywindows::commentaryWindow::syncWindow::accel,
						   actionCollection(),
						   CResMgr::displaywindows::commentaryWindow::syncWindow::actionName
											);
	m_actions.syncWindow->setToolTip(CResMgr::displaywindows::commentaryWindow::syncWindow::tooltip);
	m_actions.syncWindow->plug(mainToolBar());

	m_actions.saveText = new KAction( i18n("Save text"),
									  CResMgr::displaywindows::writeWindow::saveText::icon,
									  CResMgr::displaywindows::writeWindow::saveText::accel,
									  this, SLOT( saveCurrentText()  ),
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
										this, SLOT(restoreText()), actionCollection(),
										CResMgr::displaywindows::writeWindow::restoreText::actionName
									   );
	m_actions.restoreText->setToolTip( CResMgr::displaywindows::writeWindow::restoreText::tooltip );
	m_actions.restoreText->plug(mainToolBar());


	KToolBar* bar = new KToolBar(this);
	bar->setFullSize(true);
	addDockWindow(bar);

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
