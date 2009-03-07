/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "cdisplaywindow.h"
#include "cdisplaywindow.moc"
#include "bttoolbarpopupaction.h"
#include "btactioncollection.h"

#include "cmodulechooserbar.h"
#include "cbuttons.h"
#include "backend/keys/cswordkey.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/keychooser/bthistory.h"
#include "frontend/display/cdisplay.h"
#include "frontend/cmdiarea.h"
#include "frontend/profile/cprofilewindow.h"
#include "backend/config/cbtconfig.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "util/cresmgr.h"
#include "util/directoryutil.h"

#include <QWidget>
#include <QCloseEvent>
#include <QStringList>
#include <QDebug>
#include <QMenu>

using namespace Profile;

CDisplayWindow::CDisplayWindow(QList<CSwordModuleInfo*> modules, CMDIArea *parent)
	: QMainWindow(parent),
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
	m_displayWidget(0)
{
	qDebug("CDisplayWindow::CDisplayWindow");
	setAttribute(Qt::WA_DeleteOnClose); //we want to destroy this window when it is closed
	parent->addSubWindow(this);
	m_actionCollection = new BtActionCollection(this);
	setModules(modules);

	// Connect this to the backend module list changes
	connect(CPointers::backend(), SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)), SLOT(reload(CSwordBackend::SetupChangedReason)));
	//KMainWindow::setAttribute(Qt::WA_DeleteOnClose); //what about QMdiSubWindow?
}

CDisplayWindow::~CDisplayWindow() {
	delete m_swordKey;
	m_swordKey = 0;
}

CMDIArea* CDisplayWindow::mdi() const {
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
QList<CSwordModuleInfo*> CDisplayWindow::modules() {
	QList<CSwordModuleInfo*> mods;

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
	QWidget::setWindowTitle( windowCaption() );
	m_mdi->emitWindowCaptionChanged();
}

void CDisplayWindow::insertKeyboardActions( BtActionCollection* a ) {
	qDebug() << "CDisplayWindow::insertKeyboardActions: ac: " << a;

	QAction* actn = new QAction(QIcon(), tr("Zoom in"), 0);
	actn->setShortcut(QKeySequence::ZoomIn);
	a->addAction("zoomIn", actn);
	//a->addAction(KStandardAction::ZoomIn, "zoomIn", 0, 0);
	actn = new QAction(QIcon(), tr("Zoom out"), 0);
	actn->setShortcut(QKeySequence::ZoomOut);
	a->addAction("zoomIn", actn);
	//a->addAction(KStandardAction::ZoomOut, "zoomOut", 0, 0);
	actn = new QAction(QIcon(), tr("Close"), 0);
	actn->setShortcut(QKeySequence::Close);
	a->addAction("closeWindow", actn);
	//a->addAction(KStandardAction::Close, "closeWindow", 0, 0);
	actn = new QAction(QIcon(), tr("Select all"), 0);
	actn->setShortcut(QKeySequence::SelectAll);
	a->addAction("selectAll", actn);
	//a->addAction(KStandardAction::SelectAll, "selectAll", 0, 0);
	actn = new QAction(QIcon(), tr("Copy"), 0);
	actn->setShortcut(QKeySequence::Copy);
	a->addAction("copySelectedText", actn);
	//a->addAction(KStandardAction::Copy, "copySelectedText", 0, 0);
	actn = new QAction(QIcon(), tr("Find..."), 0);
	actn->setShortcut(QKeySequence::Find);
	a->addAction("findText", actn);
	//a->addAction(KStandardAction::Find, "findText", 0, 0);

	BtToolBarPopupAction* action = new BtToolBarPopupAction(
				QIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::displaywindows::general::backInHistory::icon)),
				tr("Back in history"),
				a
				);
	action->setShortcut(CResMgr::displaywindows::general::backInHistory::accel);
	a->addAction(CResMgr::displaywindows::general::backInHistory::actionName, action);

	action = new BtToolBarPopupAction(
				QIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::displaywindows::general::forwardInHistory::icon)),
				tr("Forward in history"),
				a
				);
	action->setShortcut(CResMgr::displaywindows::general::forwardInHistory::accel);
	a->addAction(CResMgr::displaywindows::general::forwardInHistory::actionName, action);
}

void CDisplayWindow::initActions()
{
	qDebug("CDisplayWindow::initActions");

	BtActionCollection* ac = actionCollection();

	QAction* kaction = new QAction(
				QIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::displaywindows::general::search::icon)),
				tr("Open the search dialog with the works of this window"),
				ac
				);
	kaction->setShortcut(CResMgr::displaywindows::general::search::accel);
	QObject::connect(kaction, SIGNAL(triggered()), this, SLOT(slotSearchInModules()));
	ac->addAction(CResMgr::displaywindows::general::search::actionName, kaction);

	CDisplayConnections* conn = displayWidget()->connectionsProxy();

	QAction* actn = new QAction(QIcon(), tr("Zoom in"), ac);
	actn->setShortcut(QKeySequence::ZoomIn);
	QObject::connect(actn, SIGNAL(triggered()), conn, SLOT(zoomIn()));
	ac->addAction("zoomIn", actn);
	addAction(actn);
	//a->addAction(KStandardAction::ZoomIn, "zoomIn", 0, 0);
	actn = new QAction(QIcon(), tr("Zoom out"), ac);
	actn->setShortcut(QKeySequence::ZoomOut);
	QObject::connect(actn, SIGNAL(triggered()), conn, SLOT(zoomOut()));
	ac->addAction("zoomOut", actn);
	addAction(actn);
	//a->addAction(KStandardAction::ZoomOut, "zoomOut", 0, 0);
	actn = new QAction(QIcon(), tr("Close"), ac);
	actn->setShortcut(QKeySequence::Close);
	QObject::connect(actn, SIGNAL(triggered()), this, SLOT(close()));
	ac->addAction("closeWindow", actn);
	addAction(actn);
	//a->addAction(KStandardAction::Close, "closeWindow", 0, 0);
	actn = new QAction(QIcon(), tr("Select all"), ac);
	actn->setShortcut(QKeySequence::SelectAll);
	QObject::connect(actn, SIGNAL(triggered()), conn, SLOT(selectAll()));
	ac->addAction("selectAll", actn);
	addAction(actn);
	//a->addAction(KStandardAction::SelectAll, "selectAll", 0, 0);
	actn = new QAction(QIcon(), tr("Copy"), ac);
	actn->setShortcut(QKeySequence::Copy);
	QObject::connect(actn, SIGNAL(triggered()), conn, SLOT(copySelection()));
	ac->addAction("copySelectedText", actn);
	addAction(actn);
	//a->addAction(KStandardAction::Copy, "copySelectedText", 0, 0);
	actn = new QAction(QIcon(), tr("Find..."), ac);
	actn->setShortcut(QKeySequence::Find);
	QObject::connect(actn, SIGNAL(triggered()), conn, SLOT(openFindTextDialog()));
	ac->addAction("findText", actn);
	addAction(actn);
	//a->addAction(KStandardAction::Find, "findText", 0, 0);

	BtToolBarPopupAction* popupaction = new BtToolBarPopupAction(
			QIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::displaywindows::general::backInHistory::icon)),
			tr("Back in history"),
			ac
			);
	bool ok = QObject::connect(popupaction, SIGNAL(triggered()), keyChooser()->history(), SLOT(back()));
	Q_ASSERT(ok);
	ac->addAction(CResMgr::displaywindows::general::backInHistory::actionName, popupaction);

	popupaction = new BtToolBarPopupAction(
			QIcon(util::filesystem::DirectoryUtil::getIcon(CResMgr::displaywindows::general::forwardInHistory::icon)),
			tr("Forward in history"),
			ac
			);
	ok = QObject::connect(popupaction, SIGNAL(triggered()), keyChooser()->history(), SLOT(fw()) );
	Q_ASSERT(ok);
	ac->addAction(CResMgr::displaywindows::general::forwardInHistory::actionName, popupaction);

}

/** Refresh the settings of this window. */
void CDisplayWindow::reload(CSwordBackend::SetupChangedReason) {
	//first make sure all used Sword modules are still present
	QMutableStringListIterator it(m_modules);
	while (it.hasNext()) {
		if (!backend()->findModuleByName(it.next())) {
			it.remove();
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
bool CDisplayWindow::isReady() const {
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
CKeyChooser* CDisplayWindow::keyChooser() const {
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
CModuleChooserBar* CDisplayWindow::moduleChooserBar() const {
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
void CDisplayWindow::setModules( const QList<CSwordModuleInfo*>& newModules ) {
	qDebug("CDisplayWindow::setModules");
	m_modules.clear();

	foreach (CSwordModuleInfo* mod, newModules) {
		m_modules.append(mod->name());
	}
}

/** Initialize the window. Call this method from the outside, because calling this in the constructor is not possible! */
bool CDisplayWindow::init() {
	qDebug("CDisplayWindow::init");
	initView();
	setMinimumSize( 100,100 );

	setCaption(windowCaption());
	//setup focus stuff.
	setFocusPolicy(Qt::ClickFocus);
	parentWidget()->setFocusPolicy(Qt::ClickFocus);
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
QToolBar* CDisplayWindow::mainToolBar() const {
	return m_mainToolBar;
}

/** Returns the main toolbar. */
QToolBar* CDisplayWindow::buttonsToolBar() const {
	return m_buttonsToolBar;
}

/** Sets the main toolbar. */
void CDisplayWindow::setMainToolBar( QToolBar* bar ) {
	m_mainToolBar = bar;
}

/** Sets the main toolbar. */
void CDisplayWindow::setButtonsToolBar( QToolBar* bar ) {
	m_buttonsToolBar = bar;
}

/** Returns the display settings button */
CDisplaySettingsButton* CDisplayWindow::displaySettingsButton() const {
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
	lookupSwordKey( key() );
}

void CDisplayWindow::lookupModKey( const QString& moduleName, const QString& keyName ) {
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
		//create a new window for the given module
		QList<CSwordModuleInfo*> mList;
		mList.append(m);
		mdi()->emitCreateDisplayWindow(mList, keyName);
	}
}

void CDisplayWindow::lookupKey( const QString& keyName ) {
	/* This function is called for example after a bookmark was dropped on this window
	*/
	Q_ASSERT(modules().first());

	//qDebug("CDisplayWindow::lookup: %s", keyName.latin1());
	lookupModKey(modules().first()->name(), keyName);
}

/** Update the status of the popup menu entries. */
void CDisplayWindow::updatePopupMenu() {}


///** Returns the installed popup menu. */
QMenu* CDisplayWindow::popup() {
	// qWarning("CReadWindow::popup()");
	if (!m_popupMenu) {
		m_popupMenu = new QMenu(this);
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
CDisplay* CDisplayWindow::displayWidget() const {
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

BtActionCollection* CDisplayWindow::actionCollection()
{
	return m_actionCollection;
}
