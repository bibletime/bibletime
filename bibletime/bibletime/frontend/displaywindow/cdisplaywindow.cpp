/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2006 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/



//BibleTime includes
#include "cdisplaywindow.h"
#include "creadwindow.h"
#include "cbiblereadwindow.h"
#include "ccommentaryreadwindow.h"
#include "clexiconreadwindow.h"
#include "cbookreadwindow.h"

#include "cwritewindow.h"
#include "cplainwritewindow.h"
#include "chtmlwritewindow.h"

#include "cmodulechooserbar.h"
#include "cbuttons.h"

#include "backend/cswordkey.h"

#include "frontend/keychooser/ckeychooser.h"

#include "frontend/display/cdisplay.h"

#include "frontend/cmdiarea.h"
#include "frontend/cprofilewindow.h"
#include "frontend/cbtconfig.h"

#include "frontend/searchdialog/csearchdialog.h"

#include "util/cresmgr.h"


//Qt includes

//KDE includes
#include <kdeversion.h>
#include <kaccel.h>
#include <klocale.h>

using namespace Profile;

CReadWindow* CDisplayWindow::createReadInstance(ListCSwordModuleInfo modules, CMDIArea* parent, const char* name) {
	switch (modules.first()->type()) {
		case CSwordModuleInfo::Bible:
			return new CBibleReadWindow(modules, parent, name);
		case CSwordModuleInfo::Commentary:
			return new CCommentaryReadWindow(modules, parent, name);
		case CSwordModuleInfo::Lexicon:
			return new CLexiconReadWindow(modules, parent, name);
		case CSwordModuleInfo::GenericBook:
			return new CBookReadWindow(modules, parent, name);
		default:
			qWarning("unknown module type");
		break;
	}
	return 0;
}


CWriteWindow* CDisplayWindow::createWriteInstance(ListCSwordModuleInfo modules, CMDIArea* parent, const CDisplayWindow::WriteWindowType type, const char* name) {
	if (type == HTMLWindow) {
		return new CHTMLWriteWindow(modules, parent, name);
	}
	else {
		return new CPlainWriteWindow(modules, parent, name);
	}
	return 0;
}

CDisplayWindow::CDisplayWindow(ListCSwordModuleInfo modules, CMDIArea *parent, const char *name )
#if KDE_VERSION >= 0x030200
: KMainWindow(KMainWindow::NoDCOPObject, parent, name, WDestructiveClose),
#else
: KMainWindow(parent, name, WDestructiveClose),
#endif
m_mdi(parent),
m_filterOptions(),
m_displayOptions(),
m_displaySettingsButton(0),
m_keyChooser(0),
m_swordKey(0),
m_isReady(false),
m_moduleChooserBar(0),
m_mainToolBar(0),
m_popupMenu(0),
m_displayWidget(0) {
	setModules(modules);
}

CDisplayWindow::~CDisplayWindow() {
	delete m_swordKey;
	m_swordKey = 0;
}

CMDIArea* const CDisplayWindow::mdi() const {
	return m_mdi;
}

/** Returns the right window caption. */
const QString CDisplayWindow::windowCaption() {
	if (!m_modules.count()) {
		return QString::null;
	}
	
	return QString(key()->key()).append(" (").append(m_modules.join(" | ")).append(")");
}

/** Returns the used modules as a QPtrList */
ListCSwordModuleInfo CDisplayWindow::modules() {
	ListCSwordModuleInfo mods;

	for (QStringList::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
		Q_ASSERT(backend()->findModuleByName(*it));
		if (CSwordModuleInfo* m = backend()->findModuleByName(*it)) {
			mods.append(m);
		}
	}

	return mods;
}

/** Set the window caption. */
void CDisplayWindow::setCaption( const QString&  ) {
	QWidget::setCaption( windowCaption() );
	m_mdi->emitWindowCaptionChanged();
}

void CDisplayWindow::insertKeyboardActions( KActionCollection* const a ) {
	KStdAction::zoomIn(0, 0, a, "zoomIn"); //no slot
	KStdAction::zoomOut(0, 0, a, "zoomOut"); //no slot
	KStdAction::close(0, 0, a, "closeWindow"); //no slot
	KStdAction::selectAll(0, 0, a, "selectAll");
	KStdAction::copy(0, 0, a, "copySelectedText");
	KStdAction::find(0, 0, a, "findText");

	new KToolBarPopupAction(
		i18n("Back in history"),
		CResMgr::displaywindows::general::backInHistory::icon,
		CResMgr::displaywindows::general::backInHistory::accel,
		a,
		CResMgr::displaywindows::general::backInHistory::actionName
	);
	new KToolBarPopupAction(
		i18n("Forward in history"),
		CResMgr::displaywindows::general::forwardInHistory::icon,
		CResMgr::displaywindows::general::forwardInHistory::accel,
		a, CResMgr::displaywindows::general::forwardInHistory::actionName
	);
}

void CDisplayWindow::initActions() {
	new KAction(i18n("Search"),
				CResMgr::displaywindows::general::search::icon,
				CResMgr::displaywindows::general::search::accel,
				this, SLOT(slotSearchInModules()),
				actionCollection(), CResMgr::displaywindows::general::search::actionName
			   );

	KStdAction::zoomIn(
		displayWidget()->connectionsProxy(), SLOT(zoomIn()),
		actionCollection(), "zoomIn"
	);
	KStdAction::zoomOut(
		displayWidget()->connectionsProxy(), SLOT(zoomOut()),
		actionCollection(), "zoomOut"
	);
	KStdAction::close(
		this, SLOT(close()),
		actionCollection(), "closeWindow"
	);

	KStdAction::selectAll(
		displayWidget()->connectionsProxy(), SLOT(selectAll()),
		actionCollection(), "selectAll"
	);

	KStdAction::copy(
		displayWidget()->connectionsProxy(), SLOT(copySelection()),
		actionCollection(), "copySelectedText"
	);

	KStdAction::find(
		displayWidget()->connectionsProxy(), SLOT(openFindTextDialog()),
		actionCollection(), "findText"
	);

	new KToolBarPopupAction(
		i18n("Back in history"), CResMgr::displaywindows::general::backInHistory::icon, CResMgr::displaywindows::general::backInHistory::accel,
		keyChooser(), SLOT( backInHistory() ),
		actionCollection(), CResMgr::displaywindows::general::backInHistory::actionName
	);

	new KToolBarPopupAction(
		i18n("Forward in history"), CResMgr::displaywindows::general::forwardInHistory::icon, CResMgr::displaywindows::general::forwardInHistory::accel,
		keyChooser(), SLOT( forwardInHistory() ),
		actionCollection(), CResMgr::displaywindows::general::forwardInHistory::actionName
	);

	CBTConfig::setupAccelSettings(CBTConfig::allWindows, actionCollection());
}

/** Is called when this window gets the focus or looses the focus. */
void CDisplayWindow::windowActivated( const bool hasFocus ) {
	if (accel()) {
		accel()->setEnabled(hasFocus);
	}
}

/** Refresh the settings of this window. */
void CDisplayWindow::reload() {
	//first make sure all used Sword modules are still present
	for (QStringList::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
		if (!backend()->findModuleByName(*it)) {
			it = m_modules.remove(it);
			if (it == m_modules.end()) {
				break;
			}
		}
	}
	if (m_modules.count() == 0){
		close();
		return;
	}

	if (keyChooser()) keyChooser()->setModules( modules(), false );
	
	if (m_moduleChooserBar) { //necessary for edit windows which have now chooser bar
		m_moduleChooserBar->setModules(modules());
	}
	modulesChanged();
	lookup();
}

/** Returns the filter options used by this window. */
CSwordBackend::FilterOptions& CDisplayWindow::filterOptions() {
	return m_filterOptions;
}

/** Returns the display options used by this display window. */
CSwordBackend::DisplayOptions& CDisplayWindow::displayOptions() {
	return m_displayOptions;
}

/** Sets the new display options for this window. */
void CDisplayWindow::setDisplayOptions( const CSwordBackend::DisplayOptions& displayOptions ) {
	m_displayOptions = displayOptions;
}

/** Sets the new filter options of this window. */
void CDisplayWindow::setFilterOptions( CSwordBackend::FilterOptions& filterOptions ) {
	m_filterOptions = filterOptions;
}

/** Returns true if the widget is ready for use. */
const bool CDisplayWindow::isReady() const {
	return m_isReady;
}

/** Set the ready status */
void CDisplayWindow::setReady( const bool& ready ) {
	m_isReady = ready;
}

/** Returns true if the window may be closed. */
bool CDisplayWindow::queryClose() {
	return true;
}

/** Returns the keychooser widget of this display window. */
CKeyChooser* const CDisplayWindow::keyChooser() const {
	return m_keyChooser;
}

/** Sets the keychooser widget for this display window. */
void CDisplayWindow::setKeyChooser( CKeyChooser* ck ) {
	m_keyChooser = ck;
}

/** Returns the key of this display window. */
CSwordKey* CDisplayWindow::key() const {
	Q_ASSERT( m_swordKey );
	return m_swordKey;
}

/** Sets the new sword key. */
void CDisplayWindow::setKey( CSwordKey* key ) {
	Q_ASSERT( key );
	m_swordKey = key;
}

void CDisplayWindow::modulesChanged() {
	if (moduleChooserBar()) { //necessary for write windows
		setModules( m_moduleChooserBar->getModuleList() );
	}

	if (!modules().count()) {
		close();
	}
	else {
		if (displaySettingsButton()) {
			displaySettingsButton()->reset(modules());
		}

		key()->module(modules().first());
		keyChooser()->setModules(modules());
	}
}

/** Returns the module chooser bar. */
CModuleChooserBar* const CDisplayWindow::moduleChooserBar() const {
	return m_moduleChooserBar;
}

/** Sets the module chooser bar. */
void CDisplayWindow::setModuleChooserBar( CModuleChooserBar* bar ) {
	if (m_moduleChooserBar) {
		disconnect(m_moduleChooserBar, SIGNAL(sigChanged()), this, SLOT(modulesChanged()));
	}
 	
	//if a new bar should be set!
	if (bar) {
		m_moduleChooserBar = bar;
		connect(bar, SIGNAL(sigChanged()), SLOT(modulesChanged()));
	}
}

/** Sets the modules. */
void CDisplayWindow::setModules( const ListCSwordModuleInfo& newModules ) {
	m_modules.clear();

	ListCSwordModuleInfo::const_iterator end_it = newModules.end();
	for (ListCSwordModuleInfo::const_iterator it(newModules.begin()); it != end_it; ++it) {
		m_modules.append((*it)->name());
	}
}

/** Initialize the window. Call this method from the outside, because calling this in the constructor is not possible! */
const bool CDisplayWindow::init() {
	//BibleTime::createReadDisplayWindow calls the show() method
	initView();
	setMinimumSize( 250,200 );

	setCaption(windowCaption());
	//setup focus stuff.
	setFocusPolicy(QWidget::ClickFocus);
	parentWidget()->setFocusPolicy(QWidget::ClickFocus);

	initActions();
	initToolbars();
	initConnections();
	setupPopupMenu();

	m_filterOptions = CBTConfig::getFilterOptionDefaults();
	m_displayOptions = CBTConfig::getDisplayOptionDefaults();
	if (displaySettingsButton()) {
		displaySettingsButton()->reset(modules());
	}

	setReady(true);
	return true;
}

/** Returns the main toolbar. */
KToolBar* const CDisplayWindow::mainToolBar() const {
	return m_mainToolBar;
}

/** Returns the main toolbar. */
KToolBar* const CDisplayWindow::buttonsToolBar() const {
	return m_buttonsToolBar;
}

/** Sets the main toolbar. */
void CDisplayWindow::setMainToolBar( KToolBar* bar ) {
	m_mainToolBar = bar;
}

/** Sets the main toolbar. */
void CDisplayWindow::setButtonsToolBar( KToolBar* bar ) {
	m_buttonsToolBar = bar;
}

/** Returns the display settings button */
CDisplaySettingsButton* const CDisplayWindow::displaySettingsButton() const {
	return m_displaySettingsButton;
}

/** Sets the display settings button. */
void CDisplayWindow::setDisplaySettingsButton( CDisplaySettingsButton* button ) {
	if (m_displaySettingsButton)
		disconnect(m_displaySettingsButton, SIGNAL( sigChanged() ),this, SLOT(lookup() ));

	m_displaySettingsButton = button;
	connect(m_displaySettingsButton, SIGNAL(sigChanged()),this, SLOT(lookup()));
}

/** Lookup the current key. Used to refresh the display. */
void CDisplayWindow::lookup() {
	lookup( key() );
}

void CDisplayWindow::lookup( const QString& moduleName, const QString& keyName ) {
	Q_ASSERT(isReady());
	if (!isReady()) {
		return;
	}

	CSwordModuleInfo* m = backend()->findModuleByName(moduleName);
	Q_ASSERT(m);
	if (!m) {
		return;
	}

	//ToDo: check for containsRef compat
	if (m && modules().contains(m)) {
		key()->key(keyName);
		keyChooser()->setKey(key()); //the key chooser does send an update signal
	}
	else { //given module not displayed in this window
		//if the module is displayed in another display window we assume a wrong drop
		bool found = false;
		//create a new window for the given module
		ListCSwordModuleInfo mList;
		mList.append(m);
		mdi()->emitCreateDisplayWindow(mList, keyName);
	}
}

void CDisplayWindow::lookup( const QString& keyName ) {
	/* This function is called for example after a bookmark was dropped on this window
	*/
	Q_ASSERT(modules().first());

	//qDebug("CDisplayWindow::lookup: %s", keyName.latin1());
	lookup(modules().first()->name(), keyName);
}

/** Update the status of the popup menu entries. */
void CDisplayWindow::updatePopupMenu() {}


///** Returns the installed popup menu. */
KPopupMenu* const CDisplayWindow::popup() {
	// qWarning("CReadWindow::popup()");
	if (!m_popupMenu) {
		m_popupMenu = new KPopupMenu(this);
		connect(m_popupMenu, SIGNAL(aboutToShow()), this, SLOT(updatePopupMenu()));
		if (displayWidget()) {
			displayWidget()->installPopup(m_popupMenu);
		}
		/*   else {
		    qWarning("CDisplayWindow:: can't instal popup menu");
		    }*/
	}
	return m_popupMenu;
}

/** Returns the display widget used by this implementation of CDisplayWindow. */
CDisplay* const CDisplayWindow::displayWidget() const {
	Q_ASSERT(m_displayWidget);
	return m_displayWidget;
}

/** Sets the display widget used by this display window. */
void CDisplayWindow::setDisplayWidget( CDisplay* newDisplay ) {
	m_displayWidget = newDisplay;
}

void CDisplayWindow::closeEvent(QCloseEvent* e) {
	if (!queryClose()) {
		e->ignore();
	}
	else {
		e->accept();
	}
}

void CDisplayWindow::slotSearchInModules() {
	Search::CSearchDialog::openDialog(modules());
}

void CDisplayWindow::printAll() {
	m_displayWidget->connectionsProxy()->printAll( m_displayOptions, m_filterOptions);
}

void CDisplayWindow::printAnchorWithText() {
	m_displayWidget->connectionsProxy()->printAnchorWithText( m_displayOptions, m_filterOptions);
}

