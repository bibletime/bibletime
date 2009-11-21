/*********
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2008 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

#include "frontend/displaywindow/cdisplaywindow.h"

#include <QCloseEvent>
#include <QDebug>
#include <QMenu>
#include <QStringList>
#include <QWidget>
#include "backend/config/cbtconfig.h"
#include "backend/keys/cswordkey.h"
#include "bibletime.h"
#include "frontend/cmdiarea.h"
#include "frontend/display/cdisplay.h"
#include "frontend/displaywindow/bttoolbarpopupaction.h"
#include "frontend/displaywindow/btactioncollection.h"
//#include "frontend/displaywindow/cmodulechooserbar.h"
#include "frontend/displaywindow/btmodulechooserbar.h"
#include "frontend/displaywindow/cbuttons.h"
#include "frontend/keychooser/ckeychooser.h"
#include "frontend/keychooser/bthistory.h"
#include "frontend/profile/cprofilewindow.h"
#include "frontend/searchdialog/csearchdialog.h"
#include "util/directory.h"
#include "util/cresmgr.h"


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
        m_displayWidget(0) {
    qDebug() << "CDisplayWindow::CDisplayWindow";
    setAttribute(Qt::WA_DeleteOnClose); //we want to destroy this window when it is closed
    parent->addSubWindow(this);
    m_actionCollection = new BtActionCollection(this);
    setModules(modules);

    // Connect this to the backend module list changes
    connect(CPointers::backend(), SIGNAL(sigSwordSetupChanged(CSwordBackend::SetupChangedReason)), SLOT(reload(CSwordBackend::SetupChangedReason)));
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
    qDebug() << "CDisplayWindow::modules";
//     QList<CSwordModuleInfo*> mods;
// 
//     for (QStringList::iterator it = m_modules.begin(); it != m_modules.end(); ++it) {
//         Q_ASSERT(backend()->findModuleByName(*it));
//         if (CSwordModuleInfo* m = backend()->findModuleByName(*it)) {
//             mods.append(m);
//         }
//     }

    return CPointers::backend()->getPointerList(m_modules);
}

void CDisplayWindow::insertKeyboardActions( BtActionCollection* a ) {
    namespace DU = util::directory;

    qDebug() << "CDisplayWindow::insertKeyboardActions: ac: " << a;

    QAction* actn = new QAction(QIcon(), tr("Zoom in"), a);
    actn->setShortcut(QKeySequence::ZoomIn);
    a->addAction("zoomIn", actn);

    actn = new QAction(QIcon(), tr("Zoom out"), a);
    actn->setShortcut(QKeySequence::ZoomOut);
    a->addAction("zoomOut", actn);

    actn = new QAction(QIcon(), tr("Close"), a);
    actn->setShortcut(QKeySequence::Close);
    a->addAction("closeWindow", actn);

    actn = new QAction(QIcon(), tr("Select all"), a);
    actn->setShortcut(QKeySequence::SelectAll);
    a->addAction("selectAll", actn);

    actn = new QAction(QIcon(), tr("Copy"), a);
    actn->setShortcut(QKeySequence::Copy);
    a->addAction("copySelectedText", actn);

    actn = new QAction(QIcon(), tr("Find..."), a);
    actn->setShortcut(QKeySequence::Find);
    a->addAction("findText", actn);

    actn = new QAction(QIcon(), tr("Change location"), a);
    actn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    a->addAction("openLocation", actn);

    actn = new QAction(QIcon(DU::getIcon(CResMgr::displaywindows::general::search::icon)),
                       tr("Search with works of this window"), a);
    actn->setShortcut(CResMgr::displaywindows::general::search::accel);
    a->addAction(CResMgr::displaywindows::general::search::actionName, actn);

    BtToolBarPopupAction* action = new BtToolBarPopupAction(
        QIcon(DU::getIcon(CResMgr::displaywindows::general::backInHistory::icon)),
        tr("Back in history"),
        a
    );
    action->setShortcut(CResMgr::displaywindows::general::backInHistory::accel);
    a->addAction(CResMgr::displaywindows::general::backInHistory::actionName, action);

    action = new BtToolBarPopupAction(
        QIcon(DU::getIcon(CResMgr::displaywindows::general::forwardInHistory::icon)),
        tr("Forward in history"),
        a
    );
    action->setShortcut(CResMgr::displaywindows::general::forwardInHistory::accel);
    a->addAction(CResMgr::displaywindows::general::forwardInHistory::actionName, action);
}

void CDisplayWindow::initActions() {
    qDebug() << "CDisplayWindow::initActions";

    BtActionCollection* ac = actionCollection();

    CDisplayWindow::insertKeyboardActions(ac);

    QAction* actn = ac->action(CResMgr::displaywindows::general::search::actionName);
    QObject::connect(actn, SIGNAL(triggered()), this, SLOT(slotSearchInModules()));

    CDisplayConnections* conn = displayWidget()->connectionsProxy();

    actn = ac->action("openLocation");
    QObject::connect(actn, SIGNAL(triggered()), this, SLOT(setFocusKeyChooser()));
    addAction(actn);

    actn = ac->action("zoomIn");
    QObject::connect(actn, SIGNAL(triggered()), conn, SLOT(zoomIn()));
    addAction(actn);

    actn = ac->action("zoomOut");
    QObject::connect(actn, SIGNAL(triggered()), conn, SLOT(zoomOut()));
    addAction(actn);

    actn = ac->action("closeWindow");
    QObject::connect(actn, SIGNAL(triggered()), this, SLOT(close()));
    addAction(actn);

    actn = ac->action("selectAll");
    QObject::connect(actn, SIGNAL(triggered()), conn, SLOT(selectAll()));
    addAction(actn);

    actn = ac->action("copySelectedText");
    QObject::connect(actn, SIGNAL(triggered()), conn, SLOT(copySelection()));
    addAction(actn);

    actn = ac->action("findText");
    QObject::connect(actn, SIGNAL(triggered()), conn, SLOT(openFindTextDialog()));
    addAction(actn);

    QAction* popupaction = ac->action(CResMgr::displaywindows::general::backInHistory::actionName);
    bool ok = QObject::connect(popupaction, SIGNAL(triggered()), keyChooser()->history(), SLOT(back()));
    Q_ASSERT(ok);
    addAction(popupaction);

    popupaction = ac->action(CResMgr::displaywindows::general::forwardInHistory::actionName);
    ok = QObject::connect(popupaction, SIGNAL(triggered()), keyChooser()->history(), SLOT(fw()) );
    Q_ASSERT(ok);
    addAction(popupaction);

    CBTConfig::setupAccelSettings(CBTConfig::allWindows, ac);
}

/** Refresh the settings of this window. */
void CDisplayWindow::reload(CSwordBackend::SetupChangedReason) {
    qDebug()<<"CDisplayWindow::reload";
    //first make sure all used Sword modules are still present
    QMutableStringListIterator it(m_modules);
    while (it.hasNext()) {
        if (!backend()->findModuleByName(it.next())) {
            it.remove();
        }
    }

    if (m_modules.count() == 0) {
        close();
        return;
    }

    if (keyChooser()) keyChooser()->setModules( modules(), false );

    //if (m_moduleChooserBar) { //necessary for edit windows which have now chooser bar
        //m_moduleChooserBar->setModules(modules());
    //}
    //modulesChanged();
    lookup();

    CBTConfig::setupAccelSettings(CBTConfig::allWindows, actionCollection());
    CBTConfig::setupAccelSettings(CBTConfig::readWindow, actionCollection());
    qDebug() << "CDisplayWindow::reload emits sigModuleListSet...";
    emit sigModuleListSet(m_modules);
}

void CDisplayWindow::slotAddModule(int index, QString module) {
    qDebug() << "CDisplayWindow::slotAddModule";
    m_modules.insert(index, module);
    lookup();
    //emit sigModuleAdded(index, module);
    modulesChanged();
    emit sigModuleListChanged();
}

void CDisplayWindow::slotReplaceModule(int index, QString newModule) {
    qDebug() << "CDisplayWindow::slotReplaceModule" << m_modules.at(index) << "with" << newModule;
    m_modules.replace(index, newModule);
    qDebug() << "window's new module list:" << m_modules;
    lookup();
    //emit sigModuleReplaced(index, newModule);
    modulesChanged();
    emit sigModuleListChanged();
}

void CDisplayWindow::slotRemoveModule(int index) {
    qDebug() << "CDisplayWindow::slotRemoveModule";
    m_modules.removeAt(index);
    lookup();
    //emit sigModuleRemoved(index);
    modulesChanged();
    emit sigModuleListChanged();
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
    // this would only set the stringlist again 
    //if (moduleChooserBar()) { //necessary for write windows
         //setModules( m_moduleChooserBar->getModuleList() );
     //}
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
BtModuleChooserBar* CDisplayWindow::moduleChooserBar() const {
    return m_moduleChooserBar;
}

/** Sets the module chooser bar. */
void CDisplayWindow::setModuleChooserBar( BtModuleChooserBar* bar ) {
    qDebug() << "CDisplayWindow::setModuleChooserBar";
    if (m_moduleChooserBar) {
        //disconnect(m_moduleChooserBar, SIGNAL(sigChanged()), this, SLOT(modulesChanged()));
        //disconnect all signals
        //how is this situation possible and why? When the old bar is deleted? Shouldn't it be
        // deleted here? If it's deleted the connections are disconnected automatically by Qt.
        m_moduleChooserBar->deleteLater();
    }

    //if a new bar should be set!
    if (bar) {
        m_moduleChooserBar = bar;
        //connect(bar, SIGNAL(sigChanged()), SLOT(modulesChanged()));
        //connect all signals
        //connect(bar, SIGNAL(sigModuleAdd(int,QString)), SLOT(slotAddModule(int,QString)));
        //connect(bar, SIGNAL(sigModuleRemove(int)), SLOT(slotRemoveModule(int)));
        //connect(bar, SIGNAL(sigModuleReplace(int,QString)), SLOT(slotReplaceModule(int,QString)));
        //connect(this, SIGNAL(sigModuleListSet(QStringList)), bar, SLOT(slotRecreate()));
        
        //connect(this, SIGNAL(sigModuleAdded(int, QString)), bar, SLOT(slotAddModule(int,QString)));
        //connect(this, SIGNAL(sigModuleRemoved(int)), bar, SLOT(slotRemoveModule(int)));
        //connect(this, SIGNAL(sigModuleReplaced(int, QString)), bar, SLOT(slotReplaceModule(int,QString)));
    }
}

/** Sets the modules. */
void CDisplayWindow::setModules( const QList<CSwordModuleInfo*>& newModules ) {
    qDebug() << "CDisplayWindow::setModules";
    m_modules.clear();

    foreach (CSwordModuleInfo* mod, newModules) {
        m_modules.append(mod->name());
    }
}

/** Initialize the window. Call this method from the outside, because calling this in the constructor is not possible! */
bool CDisplayWindow::init() {
    qDebug() << "CDisplayWindow::init";
    initView();
    setMinimumSize( 100, 100 );

    setWindowTitle(windowCaption());
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
        disconnect(m_displaySettingsButton, SIGNAL( sigChanged() ), this, SLOT(lookup() ));

    m_displaySettingsButton = button;
    connect(m_displaySettingsButton, SIGNAL(sigChanged()), this, SLOT(lookup()));
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

    /// \todo check for containsRef compat
    if (m && modules().contains(m)) {
        key()->key(keyName);
        keyChooser()->setKey(key()); //the key chooser does send an update signal
    }
    else { 	//given module not displayed in this window
        //if the module is displayed in another display window we assume a wrong drop
        //create a new window for the given module
        QList<CSwordModuleInfo*> mList;
        mList.append(m);

        Q_ASSERT(qobject_cast<BibleTime*>(mdi()->parent()) != 0);
        BibleTime *mainWindow(static_cast<BibleTime*>(mdi()->parent()));
        mainWindow->createReadDisplayWindow(mList, keyName);
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
void CDisplayWindow::updatePopupMenu() {
}


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

BtActionCollection* CDisplayWindow::actionCollection() {
    return m_actionCollection;
}

void CDisplayWindow::setFocusKeyChooser() {
    keyChooser()->setFocus();
}

